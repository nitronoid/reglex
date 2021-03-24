# -*- mode: python -*-
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_compilation_db",
    strip_prefix = "bazel-compilation-database-master",
    urls = [
        "https://github.com/grailbio/bazel-compilation-database/archive/master.tar.gz"
    ],
)

new_git_repository(
    name = "magic-enum",
    build_file = "//bazel/thirdparty:BUILD.magic-enum",
    branch = "v0.7.1",
    remote = "https://github.com/Neargye/magic_enum",
)
