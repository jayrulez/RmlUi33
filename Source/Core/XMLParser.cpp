/*
 * This source file is part of RmlUi, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://github.com/mikke89/RmlUi
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2019 The RmlUi Team, and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "DocumentHeader.h"
#include "../../Include/RmlUi/Core/Log.h"
#include "../../Include/RmlUi/Core/Profiling.h"
#include "../../Include/RmlUi/Core/Stream.h"
#include "../../Include/RmlUi/Core/Types.h"
#include "../../Include/RmlUi/Core/XMLNodeHandler.h"
#include "../../Include/RmlUi/Core/URL.h"
#include "../../Include/RmlUi/Core/XMLParser.h"

namespace Rml {
namespace Core {

using NodeHandlers = UnorderedMap< String, SharedPtr<XMLNodeHandler> > ;
static NodeHandlers node_handlers;
static SharedPtr<XMLNodeHandler> default_node_handler;

XMLParser::XMLParser(Element* root)
{
	RegisterCDATATag("script");

	// Add the first frame.
	ParseFrame frame;
	frame.node_handler = nullptr;
	frame.child_handler = nullptr;
	frame.element = root;
	frame.tag = "";
	stack.push(frame);

	active_handler = nullptr;

	header = new DocumentHeader();
}

XMLParser::~XMLParser()
{
	delete header;
}

// Registers a custom node handler to be used to a given tag.
XMLNodeHandler* XMLParser::RegisterNodeHandler(const String& _tag, SharedPtr<XMLNodeHandler> handler)
{
	String tag = StringUtilities::ToLower(_tag);

	// Check for a default node registration.
	if (tag.empty())
	{
		default_node_handler = std::move(handler);
		return default_node_handler.get();
	}

	XMLNodeHandler* result = handler.get();
	node_handlers[tag] = std::move(handler);
	return result;
}

// Releases all registered node handlers. This is called internally.
void XMLParser::ReleaseHandlers()
{
	default_node_handler.reset();
	node_handlers.clear();
}

DocumentHeader* XMLParser::GetDocumentHeader()
{
	return header;
}

const URL& XMLParser::GetSourceURL() const
{
	return xml_source->GetSourceURL();
}

// Pushes the default element handler onto the parse stack.
void XMLParser::PushDefaultHandler()
{
	active_handler = default_node_handler.get();
}

bool XMLParser::PushHandler(const String& tag)
{
	NodeHandlers::iterator i = node_handlers.find(StringUtilities::ToLower(tag));
	if (i == node_handlers.end())
		return false;

	active_handler = i->second.get();
	return true;
}

/// Access the current parse frame
const XMLParser::ParseFrame* XMLParser::GetParseFrame() const
{
	return &stack.top();
}

/// Called when the parser finds the beginning of an element tag.
void XMLParser::HandleElementStart(const String& _name, const XMLAttributes& attributes)
{
	RMLUI_ZoneScoped;
	const String name = StringUtilities::ToLower(_name);

	// Check for a specific handler that will override the child handler.
	NodeHandlers::iterator itr = node_handlers.find(name);
	if (itr != node_handlers.end())
		active_handler = itr->second.get();

	// Store the current active handler, so we can use it through this function (as active handler may change)
	XMLNodeHandler* node_handler = active_handler;

	Element* element = nullptr;

	// Get the handler to handle the open tag
	if (node_handler)
	{
		element = node_handler->ElementStart(this, name, attributes);
	}

	// Push onto the stack
	ParseFrame frame;
	frame.node_handler = node_handler;
	frame.child_handler = active_handler;
	frame.element = (element ? element : stack.top().element);
	frame.tag = name;
	stack.push(frame);
}

/// Called when the parser finds the end of an element tag.
void XMLParser::HandleElementEnd(const String& _name)
{
	RMLUI_ZoneScoped;
	String name = StringUtilities::ToLower(_name);

	// Copy the top of the stack
	ParseFrame frame = stack.top();
	// Pop the frame
	stack.pop();
	// Restore active handler to the previous frame's child handler
	active_handler = stack.top().child_handler;	

	// Check frame names
	if (name != frame.tag)
	{
		Log::Message(Log::LT_ERROR, "Closing tag '%s' mismatched on %s:%d was expecting '%s'.", name.c_str(), GetSourceURL().GetURL().c_str(), GetLineNumber(), frame.tag.c_str());
	}

	// Call element end handler
	if (frame.node_handler)
	{
		frame.node_handler->ElementEnd(this, name);
	}	
}

/// Called when the parser encounters data.
void XMLParser::HandleData(const String& data)
{
	RMLUI_ZoneScoped;
	if (stack.top().node_handler)
		stack.top().node_handler->ElementData(this, data);
}

}
}
