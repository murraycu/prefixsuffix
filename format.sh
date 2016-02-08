CLANG_FORMAT="clang-format-3.7"

for DIRECTORY in .
do
    echo "Formatting code under $DIRECTORY/"
    find "$DIRECTORY" \( -name '*.h' -or -name '*.cc' -or -name '*.cpp' \) -print0 | xargs -0 "$CLANG_FORMAT" -i
done


