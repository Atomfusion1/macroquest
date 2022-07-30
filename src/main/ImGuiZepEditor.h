/*
 * MacroQuest: The extension platform for EverQuest
 * Copyright (C) 2002-2022 MacroQuest Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "zep/display.h"
#include "zep/editor.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace mq::imgui {

//============================================================================

class ZepEditor_ImGui;

class ImGuiZepEditor : public Zep::IZepComponent
{
public:
	ImGuiZepEditor();
	virtual ~ImGuiZepEditor();

	void SetFont(Zep::ZepTextType, ImFont* pFont, int pixelHeight);

	virtual Zep::ZepEditor& GetEditor() const override;
	virtual void Render(const char* id, const ImVec2& displaySize = ImVec2());
	bool IsHovered() const { return m_hovered; }
	bool m_hovered = false;
protected:
	virtual void Notify(std::shared_ptr<Zep::ZepMessage> message) override;

private:
	ZepEditor_ImGui* m_editor = nullptr;
};


} // namespace mq::imgui
