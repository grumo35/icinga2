/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2018 Icinga Development Team (https://www.icinga.com/)  *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#include "config/applyrule.hpp"
#include "base/logger.hpp"
#include <set>

using namespace icinga;

ValidatorRule::RuleVector ValidatorRule::m_Rules;

ValidatorRule::ValidatorRule(String name, std::set<Type::Ptr> targets, std::shared_ptr<Expression> expression,
	DebugInfo di)
	: m_Name(std::move(name)), m_TargetTypes(std::move(targets)), m_Expression(std::move(expression)), m_DebugInfo(std::move(di))
{ }

String ValidatorRule::GetName() const
{
	return m_Name;
}

std::set<Type::Ptr> ValidatorRule::GetTargetTypes() const
{
	return m_TargetTypes;
}

std::shared_ptr<Expression> ValidatorRule::GetExpression() const
{
	return m_Expression;
}

DebugInfo ValidatorRule::GetDebugInfo() const
{
	return m_DebugInfo;
}

void ValidatorRule::AddRule(const String& name, const std::set<Type::Ptr>& targetTypes, const std::shared_ptr<Expression>& expression,
	const DebugInfo& di)
{
	m_Rules.push_back(ValidatorRule(name, targetTypes, expression, di));
}

void ValidatorRule::EvaluateRule(const ConfigObject::Ptr& object) const
{
	ScriptFrame frame(true);
	frame.Locals->Set("validator", m_Name);
	frame.Self = object;
	m_Expression->Evaluate(frame);
}

void ValidatorRule::EvaluateRules(const ConfigObject::Ptr& object)
{
	for (const ValidatorRule& rule : m_Rules)
		if (rule.m_TargetTypes.empty() || rule.m_TargetTypes.find(object->GetReflectionType()) != rule.m_TargetTypes.end())
			rule.EvaluateRule(object);
}
