#!/bin/bash
# Automated test suite for LightKeyForge (server + client)

set -e
set -o pipefail

SERVER="./server"
CLIENT="./client"

expect() {
    local cmd="$1"
    local expected="$2"
    local output

    output=$(echo "$cmd" | $CLIENT)
    echo "> $cmd"
    echo "  Got:      $output"
    echo "  Expected: $expected"

    if [[ "$output" != "$expected" ]]; then
        echo "❌ Test failed for command: $cmd"
        kill $SERVER_PID 2>/dev/null || true
        exit 1
    fi
    echo "✅ Passed"
    echo
}

$SERVER &
SERVER_PID=$!
sleep 1

echo "Server started (PID=$SERVER_PID)"
echo "Running automated tests..."
echo

expect "PUT name Alice" "OK"
expect "GET name" "Alice"
expect "UPDATE name Bob" "OK"
expect "GET name" "Bob"
expect "GET_KEY Bob" "name"
expect "DELETE name" "OK"
expect "STATS" "Number of keys: 0"
expect "SHUTDOWN" "SHUTTING DOWN"

wait $SERVER_PID
echo "🎉 All tests passed successfully!"
