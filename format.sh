find ./src ./tests -name '*.h' -o -name '*.cpp' | xargs clang-format -i --verbose --style=file:.clang-format