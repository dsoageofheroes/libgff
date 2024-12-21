const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const libxmi2mid = b.addSharedLibrary(.{
        .name = "libxmi2mid",
        .target = target,
        .optimize = optimize,
        .version = .{.major = 1, .minor = 0, .patch = 0},
    });

    libxmi2mid.addCSourceFiles(.{
        .root = b.path("ext"),
        .files = &.{
            "xmi2mid.c",
        },
    });

    libxmi2mid.addIncludePath(b.path("ext"));
    libxmi2mid.linkLibC();

    b.installArtifact(libxmi2mid);

    const libgff = b.addSharedLibrary(.{
        .name = "gff",
        .target = target,
        .optimize = optimize,
        .version = .{.major = 0, .minor = 6, .patch = 0},
    });

    libgff.addIncludePath(b.path("include"));
    libgff.addIncludePath(b.path("ext"));

    libgff.addCSourceFiles(.{
        .root = b.path("src"),
        .files = &.{
            "char.c",
            "debug.c",
            "gff.c",
            "gfftypes.c",
            "gui.c",
            "image.c",
            "item.c",
            "manager.c",
            "name.c",
            "object.c",
            "rdff.c",
            "region.c",
            "scmd.c",
            "xmi.c",
            "gpl/gpl.c",
            //"gpl/lua.c",
            "gpl/manager.c",
            "gpl/parse.c",
            //"gpl/request.c",
            "gpl/state.c",
            "gpl/string.c",
            //"gpl/var.c",
        },
    });

    libgff.linkLibrary(libxmi2mid);

    b.installArtifact(libgff);

    const gfftool = b.addExecutable(.{
        .name = "gfftool",
        .target = target,
        .optimize = optimize,
    });

    gfftool.addCSourceFiles(.{
        .root = b.path("tools"),
        .files = &.{
            "gfftool.c",
            "gfftool-print.c",
            "gfftool-write.c",
        }
    });
    
    gfftool.linkLibrary(libgff);
    gfftool.linkLibrary(libxmi2mid);
    gfftool.addIncludePath(b.path("include"));

    b.installArtifact(gfftool);

    const run_exe = b.addRunArtifact(gfftool);

    const run_step = b.step("run", "Run hello!");
    run_step.dependOn(&run_exe.step);
}
