
local function build_client(name)
target(name)
    set_kind("static")
    set_group("Client")
    add_includedirs(".","../../Libraries/")
    set_pcxxheader("TiltedOnlinePCH.h")

    -- exclude game specifc stuff
    add_headerfiles("**.h|Games/Skyrim/**|Services/Vivox/**")
    add_files("**.cpp|Games/Skyrim/**|Services/Vivox/**")

    after_install(function(target)
        -- copy dlls
        for _, pkg_with_dlls in ipairs({"cef", "discord"}) do
            local linkdir = target:pkg(pkg_with_dlls):get("linkdirs")
            local bindir = path.join(linkdir, "..", "bin")
            os.cp(bindir, target:installdir())
        end
        -- copy ui
        local uidir = path.join(target:scriptdir(), "..", "skyrim_ui", "src")
        os.cp(path.join(uidir, "assets", "images", "cursor.dds"), path.join(target:installdir(), "bin", "assets", "images", "cursor.dds"))
        os.cp(path.join(uidir, "assets", "images", "cursor.png"), path.join(target:installdir(), "bin", "assets", "images", "cursor.png"))
        os.rm(path.join(target:installdir(), "bin", "**Tests.exe"))
    end)

    add_files("Games/Skyrim/**.cpp")
    add_headerfiles("Games/Skyrim/**.h")
    -- rather hacky:
    add_includedirs("Games/Skyrim")
    add_deps("SkyrimEncoding")
    add_deps(
        "UiProcess",
        "CommonLib",
        "BaseLib",
        "ImGuiImpl",
        "TiltedConnect",
        "TiltedReverse",
        "TiltedHooks",
        "TiltedUi",
        {inherit = true}
    )

    add_packages(
        "tiltedcore",
        "spdlog",
        "hopscotch-map",
        "cryptopp",
        "gamenetworkingsockets",
        "discord",
        "imgui",
        "cef",
        "minhook",
        "entt",
        "glm",
        "mem",
        "xbyak")

    if has_config("vivox") then
        add_files("Services/Vivox/**.cpp")
        add_headerfiles("Services/Vivox/**.h")
        add_includedirs("Services/Vivox")
        add_deps("Vivox")
        add_defines("TP_VIVOX=1")
    else
        add_defines("TP_VIVOX=0")
    end

    add_syslinks(
        "version",
        "dbghelp",
        "kernel32")
end

-- LOCAL FORK CHANGE (diverges from upstream v1.8.0): v0.2.7 -> v0.2.9.
-- This is a SECOND, separate require of tiltedcore (debug variant, built as its own
-- package instance), so bumping the pin in the root xmake.lua alone is not enough --
-- this line is the one that actually failed to link:
--   TiltedCore.lib(MimallocAllocator.cpp.obj) : error LNK2019:
--     unresolved external symbol mi_malloc_size
-- See the longer explanation on the tiltedcore entry in the root xmake.lua.
-- Keep this version in sync with the root xmake.lua.
add_requires("tiltedcore v0.2.9", {debug = true})

build_client("SkyrimTogetherClient")
