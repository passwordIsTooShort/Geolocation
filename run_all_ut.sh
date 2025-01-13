repo_root=$(git rev-parse --show-toplevel)
tests_path=${repo_root}/out/tests

testFiles=$(ls ${tests_path}/*Test)

for test in ${testFiles}; do
    ${test}
done