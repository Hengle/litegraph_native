#include "base.h"
#include <iostream>

using namespace LiteGraph;


OutputNode::OutputNode()
{
	CTOR_NODE();

	addInput("in", DataType::NUMBER);
}

OutputNode* output_node = new OutputNode();

//*****************************

ConstNumberNode::ConstNumberNode()
{
	CTOR_NODE();
	value = 4;
	addOutput("out", DataType::NUMBER);
}

void ConstNumberNode::onExecute()
{
	setOutputDataAsNumber(0, value);
}

void ConstNumberNode::onConfigure(void* json)
{
	JSON properties = getJSONObject(json, "properties");
	if (!properties)
		return;
	readJSONNumber(properties, "value", value);
}

ConstNumberNode* const_node = new ConstNumberNode();


ConstStringNode::ConstStringNode()
{
	CTOR_NODE();
	value = "";
	addOutput("out", DataType::STRING);
}

void ConstStringNode::onExecute()
{
	setOutputDataAsString(0, value.c_str());
}

void ConstStringNode::onConfigure(void* json)
{
	JSON properties = getJSONObject(json, "properties");
	if (!properties)
		return;
	readJSONString(properties, "value", value);
}

ConstStringNode* const_string_node = new ConstStringNode();


ConstDataNode::ConstDataNode()
{
	CTOR_NODE();
	value = NULL;
	addOutput("out", DataType::OBJECT);
}

ConstDataNode::~ConstDataNode()
{
	if (value == NULL)
		return;
	freeJSON(value);
}

void ConstDataNode::onExecute()
{
	setOutputDataAsPointer(0, value);
}

void ConstDataNode::onConfigure(void* json)
{
	JSON properties = getJSONObject(json, "properties");
	if (!properties)
		return;
	std::string json_str;
	if (readJSONString(properties, "value", json_str))
	{
		value = parseJSON(json_str.c_str());
	}
}

ConstDataNode* const_data_node = new ConstDataNode();


ObjectPropertyNode::ObjectPropertyNode()
{
	CTOR_NODE();
	name = "";
	addInput("in", DataType::JSON_OBJECT);
	addOutput("out", DataType::ANY);
}

void ObjectPropertyNode::onExecute()
{
	JSON input = getInputDataAsJSON(0);
	if (input)
	{
		readJSONString(input, name.c_str(), _str);
		setOutputDataAsString(0, _str.c_str());
	}
}

void ObjectPropertyNode::onConfigure(void* json)
{
	JSON properties = getJSONObject(json, "properties");
	if (!properties)
		return;
	//readJSONString(properties, "value", value);
}

ObjectPropertyNode* object_property_node = new ObjectPropertyNode();

//*****************************

GateNode::GateNode()
{
	CTOR_NODE();
	addInput("v", DataType::BOOL);
	addInput("A", DataType::NUMBER);
	addInput("B", DataType::NUMBER);
	addOutput("out", DataType::NUMBER);
}

void GateNode::onExecute()
{
	bool v = getInputDataAsBoolean(0);
	float A = getInputDataAsNumber(1);
	float B = getInputDataAsNumber(2);
	setOutputDataAsNumber(0, v ? A : B );
}

GateNode* gate_node = new GateNode();

//*****************************
WatchNode::WatchNode()
{
	CTOR_NODE();

	addInput("in", DataType::NUMBER);
}

void WatchNode::onExecute()
{
	std::cout << "Out: " << getInputDataAsString(0) << std::endl;
}

WatchNode* watch_node = new WatchNode();


ConsoleNode::ConsoleNode()
{
	CTOR_NODE();

	addInput("Log", DataType::NUMBER);
}

void ConsoleNode::onExecute()
{
}

void ConsoleNode::onAction(int slot_index, LEvent& event)
{
	LSlot* slot = inputs[slot_index];
	if(slot)
		std::cout << slot->name << ": " << event.type << std::endl;
}

ConsoleNode* console_node = new ConsoleNode();

//*************************

TimeNode::TimeNode()
{
	CTOR_NODE();

	addOutput("ms", DataType::NUMBER);
	addOutput("sec", DataType::NUMBER);
}

void TimeNode::onExecute()
{
	setOutputDataAsNumber( 0, graph->time * 1000 );
	setOutputDataAsNumber( 1, graph->time );
}

TimeNode* time_node = new TimeNode();

//*************************

ConditionNode::ConditionNode()
{
	CTOR_NODE();
	OP = ConditionType::LESS;

	addInput("A", DataType::NUMBER);
	addInput("B", DataType::NUMBER);
	addOutput("true", DataType::BOOL);
	addOutput("false", DataType::BOOL);
}

void ConditionNode::onExecute()
{
	double A = getInputDataAsNumber(0);
	double B = getInputDataAsNumber(1);
	bool C = false;
	switch (OP)
	{
		case ConditionType::NEQUAL: C = A != B; break;
		case ConditionType::EQUAL: C = A == B; break;
		case ConditionType::LEQUAL: C = A <= B; break;
		case ConditionType::LESS: C = A < B; break;
		case ConditionType::GREATER: C = A > B; break;
		case ConditionType::GEQUAL: C = A >= B; break;
		case ConditionType::AND: C = A != 0 && B != 0; break;
		case ConditionType::OR: C = A != 0 || B != 0; break;
		default: break;
	}
	setOutputDataAsBoolean(0, C);
	setOutputDataAsBoolean(1, !C);
}

void ConditionNode::onConfigure(void* json)
{
	JSON properties = getJSONObject(json, "properties");
	if (!properties)
		return;
	readJSONNumber(properties, "A", A);
	readJSONNumber(properties, "B", B);
	std::string cond;
	if (readJSONString(properties, "OP", cond))
	{
		if(cond == "<")
			OP = ConditionType::LESS;
		else if (cond == "<=")
			OP = ConditionType::LEQUAL;
		else if (cond == ">")
			OP = ConditionType::GREATER;
		else if (cond == ">=")
			OP = ConditionType::GEQUAL;
		else if (cond == "==")
			OP = ConditionType::EQUAL;
		else if (cond == "!=")
			OP = ConditionType::NEQUAL;
		else if (cond == "||")
			OP = ConditionType::OR;
		else if (cond == "&&")
			OP = ConditionType::AND;
	}
}

ConditionNode* condition_node = new ConditionNode();




TrigonometryNode::TrigonometryNode()
{
	CTOR_NODE();
	amplitude = 1;
	offset = 0;
	addInput("v", DataType::NUMBER);
	addInput("sin", DataType::NUMBER);
}

void TrigonometryNode::onExecute()
{
	float v = 0;
	if (isInputConnected(0))
		v = getInputDataAsNumber(0);
	for (int i = 0; i < outputs.size(); ++i)
	{
		LSlot* slot = outputs[i];
		if (!slot->isConnected())
			continue;
		if (slot->name == "sin")
			setOutputDataAsNumber(i, sin(v) * amplitude + offset);
		else if (slot->name == "cos")
			setOutputDataAsNumber(i, cos(v) * amplitude + offset);
		else if (slot->name == "tan")
			setOutputDataAsNumber(i, tan(v) * amplitude + offset);
		else if (slot->name == "asin")
			setOutputDataAsNumber(i, asin(v) * amplitude + offset);
		else if (slot->name == "acos")
			setOutputDataAsNumber(i, asin(v) * amplitude + offset);
		else if (slot->name == "atan")
			setOutputDataAsNumber(i, asin(v) * amplitude + offset);
	}
}

void TrigonometryNode::onConfigure(void* json)
{
	JSON properties = getJSONObject(json, "properties");
	if (!properties)
		return;
	readJSONNumber(properties, "amplitude", amplitude);
	readJSONNumber(properties, "offset", offset);
}


TrigonometryNode* trigonometry_node = new TrigonometryNode();

//**************************************


TimerNode::TimerNode()
{
	CTOR_NODE();
	addOutput("on_tick", DataType::EVENT);
	_next_trigger = 0;
	interval = 1000;
}

void TimerNode::onExecute()
{
	if (isInputConnected(0))
		interval = getInputDataAsNumber(0);

	if (graph->time < _next_trigger)
		return;
	_next_trigger = graph->time + interval * 0.001;
	trigger(0, LEvent("tick"));
}

void TimerNode::onConfigure(void* json)
{
	_next_trigger = 0;
	JSON properties = getJSONObject(json, "properties");
	if (!properties)
		return;
	readJSONNumber(properties, "interval", interval);
}


TimerNode* timer_node = new TimerNode();