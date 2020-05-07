#!/usr/bin/env bash
find src -name '*.cc' | xargs clang-format -i
find src -name '*.hh' | xargs clang-format -i
find src -name '*.h' | xargs clang-format -i
find src -name '*.cpp' | xargs clang-format -i

find tests -name '*.cc' | xargs clang-format -i
find tests -name '*.hh' | xargs clang-format -i
find tests -name '*.h' | xargs clang-format -i
find tests -name '*.cpp' | xargs clang-format -i
