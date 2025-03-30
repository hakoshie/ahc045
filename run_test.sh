#!/bin/bash

# 引数がない場合はエラー
if [ -z "$1" ]; then
    echo "Usage: $0 <number>"
    exit 1
fi

# 引数をゼロ埋めした4桁の数字に変換
INPUT_FILE=$(printf "in/%04d.txt" "$1")

# テスト実行
g++ main.cpp
cd ./tools && cargo run -r --bin tester ../a.out < "$INPUT_FILE" >../output.txt && cd ..
