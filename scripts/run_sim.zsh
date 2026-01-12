#!/bin/zsh
# Usage: ./run_sim.zsh <module_name>
# Example: ./run_sim.zsh alu

MODULE_NAME=$1
SRC_DIR="../src"
TEST_DIR="../test"
BUILD_DIR="../build"

if [ -z "$MODULE_NAME" ]; then
    echo "Error: Please specify a module name (e.g., ./run_sim.zsh alu)"
    exit 1
fi

echo "Simulating: $MODULE_NAME"
mkdir -p $BUILD_DIR

# Compile
iverilog -o "${BUILD_DIR}/${MODULE_NAME}_sim" \
    "${SRC_DIR}/${MODULE_NAME}.v" \
    "${TEST_DIR}/tb_${MODULE_NAME}.v"

if [ $? -eq 0 ]; then
    cd $BUILD_DIR
    vvp "${MODULE_NAME}_sim"
    gtkwave waves.vcd &
else
    echo "Compilation failed."
fi
