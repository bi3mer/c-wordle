const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const raylib_dep = b.dependency("raylib", .{ .target = target, .optimize = optimize });

    const words_h = generateWordsHeader(b);

    const exe = b.addExecutable(.{
        .name = "wordle",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        }),
    });

    exe.root_module.addIncludePath(raylib_dep.path("src"));
    exe.root_module.addIncludePath(words_h.dirname());

    if (b.build_root.handle.openDir("src", .{ .iterate = true })) |dir| {
        var d = dir;
        defer d.close();
        var iter = d.iterate();
        while (iter.next() catch null) |entry| {
            if (entry.kind == .file and std.mem.endsWith(u8, entry.name, ".c")) {
                exe.root_module.addCSourceFile(.{
                    .file = b.path(b.fmt("src/{s}", .{entry.name})),
                    .flags = &.{ "-std=c99", "-Wall", "-Wextra", "-pedantic" },
                });
            }
        }
    } else |_| {}

    exe.root_module.linkLibrary(raylib_dep.artifact("raylib"));
    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| run_cmd.addArgs(args);
    b.step("run", "Run the game").dependOn(&run_cmd.step);
}

fn generateWordsHeader(b: *std.Build) std.Build.LazyPath {
    const input = b.build_root.handle.readFileAlloc(b.allocator, "assets/words.txt", 1024 * 1024) catch @panic("Could not read words.txt");

    // Count words
    var count: usize = 0;
    var iter = std.mem.splitScalar(u8, input, '\n');
    while (iter.next()) |line| {
        const trimmed = std.mem.trim(u8, line, &.{ ' ', '\r', '\t' });
        if (trimmed.len > 0) count += 1;
    }

    // Build output
    var out = std.ArrayListUnmanaged(u8){};
    out.appendSlice(b.allocator, "#ifndef WORDS_H\n#define WORDS_H\n\n") catch unreachable;
    out.appendSlice(b.allocator, b.fmt("static const int NUM_WORDS = {d};\n\n", .{count})) catch unreachable;
    out.appendSlice(b.allocator, "static const char* WORDS[] = {\n") catch unreachable;

    iter = std.mem.splitScalar(u8, input, '\n');
    while (iter.next()) |line| {
        const trimmed = std.mem.trim(u8, line, &.{ ' ', '\r', '\t' });
        if (trimmed.len > 0) {
            out.appendSlice(b.allocator, b.fmt("    \"{s}\",\n", .{trimmed})) catch unreachable;
        }
    }

    out.appendSlice(b.allocator, "};\n\n#endif\n") catch unreachable;

    const wf = b.addWriteFiles();
    return wf.add("words.h", out.items);
}
