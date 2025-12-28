
#include "net.hpp"
#include "backend.hpp"
#include "thread"
window fake_deep_payload;
auto Deep = std::make_unique<net>();
//
static int64_t i64MaxSizePacket = 0xFFFF;
static int64_t i64MaxThreads = std::jthread::hardware_concurrency();
static std::string sIPAddress = "127.0.0.1";
static int64_t i64Port = 443;
//
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int main() {
    fake_deep_payload.SetSize(400, 260);
    fake_deep_payload.setTitleText(L"FakeDeepPayload v0.6 (RmFrZURlZX)");
    fake_deep_payload.createWindow(WndProc);
    fake_deep_payload.EnableCustomRender(true);
    fake_deep_payload.initDeviceGL();
    fake_deep_payload.SetDarkStyle();
    ImGuiIO io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    while (fake_deep_payload.render) {
        fake_deep_payload.NewFrame();
        fake_deep_payload.getMessage();
        //
        Deep->bMultiThread = fake_deep_payload.bIsMultiThread;
        //
        ImGui::Begin("Hello",nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
        ImGui::SetWindowSize({ (float)fake_deep_payload.g_Width - 1,(float)fake_deep_payload.g_Height - 1 });
        ImGui::SetWindowPos({ 0,0 });
        ImGui::TextColored(RGBA(144, 159, 210, 255), std::format("{}",std::string(fake_deep_payload.WindowTitle.begin(), fake_deep_payload.WindowTitle.end())).c_str());
        ImGui::Separator();
        ImGui::InputText("IP Addr", &sIPAddress);
        ImGui::InputInt64("port", &i64Port,1,100,0);
        ImGui::InputInt64("size_packet", &i64MaxSizePacket, 1, 100, 0);
        ImGui::InputInt64("count packets", &Deep->max_iteration, 1, 100, 0);
        ImGui::Checkbox("Enable MultiThread: ", &fake_deep_payload.bIsMultiThread); if (fake_deep_payload.bIsMultiThread) {
            ImGui::SameLine(); ImGui::Text(std::format("cpu_threads: {}", std::jthread::hardware_concurrency()).c_str());
        }
        if (ImGui::Button("Start", { float(fake_deep_payload.g_Width - 15),40 })) {
            Deep->PACKET_SIZE = i64MaxSizePacket;
            Deep->target_ip = sIPAddress.c_str();
            Deep->target_port = int(i64Port);
            Deep->sendDeep();
        }
        ImGui::TextColored(RGBA(255, 30, 144, 255), "Writtein C++20 by HCPP");
        ImGui::TextLinkOpenURL("github.com/hcpp20334", "https://github.com/hcpp20334");
        ImGui::End();
        fake_deep_payload.viewport();
    }
    fake_deep_payload.shutdownDevice();

}
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            fake_deep_payload.g_Width = LOWORD(lParam);
            fake_deep_payload.g_Height = HIWORD(lParam);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        exit(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}//