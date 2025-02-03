#pragma once
#include "Atom/Graphics/Renderer2D.h"
#include "Atom/Graphics/Texture.h"

#include <imgui.h>

namespace Atom::ImGui
{

	enum class OverlayLocation
	{
		TopRight = 1,
		BottomLeft = 2,
		BottomRight = 3,
		TopLeft = 4
	}; 

	inline static bool BeginOverlay(const char* name, OverlayLocation location = OverlayLocation::TopRight, const float padding = 10.0f) 
	{
		int32_t loc = (int32_t)location;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		const ImGuiViewport* viewport = ::ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (loc & 1) ? (work_pos.x + work_size.x - padding) : (work_pos.x + padding);
		window_pos.y = (loc & 2) ? (work_pos.y + work_size.y - padding) : (work_pos.y + padding);
		window_pos_pivot.x = (loc & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (loc & 2) ? 1.0f : 0.0f;
		::ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		::ImGui::SetNextWindowViewport(viewport->ID);
		window_flags |= ImGuiWindowFlags_NoMove;
		::ImGui::SetNextWindowBgAlpha(0.35f);

		return ::ImGui::Begin(name, nullptr, window_flags); 
	}

	inline static void DrawRenderer2DStats(const Atom::Renderer2DStatistics& stats)
	{
		::ImGui::Text("Renderer2D statistics:");
		::ImGui::Text("Quad Count: %d", stats.QuadCount);
		::ImGui::Text(" - Vertices: %d", stats.GetQuadVertexCount());
		::ImGui::Text(" - Indices: %d", stats.GetQuadIndexCount());

		::ImGui::Text("Line Count: %d", stats.LineCount);
		::ImGui::Text(" - Vertices: %d", stats.GetLineVertexCount());
		::ImGui::Text(" - Indices: %d", stats.GetLineIndexCount());

		::ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	}

	inline static void Image(RenderTexture* renderTexture, const ImVec2& image_size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0))
	{
		::ImGui::Image((ImTextureID)renderTexture->GetRaw(), image_size, uv0, uv1, tint_col, border_col);
	}

}
