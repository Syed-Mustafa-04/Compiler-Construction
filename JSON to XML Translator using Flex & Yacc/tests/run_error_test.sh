#!/bin/sh
# Error handling test for json2xml: invalid JSON should exit non-zero and print a syntax error.
set -eu

if ./json2xml < tests/test_error.json > /dev/null 2> tests/test_error.out; then
  echo "FAIL: expected json2xml to fail on invalid JSON"
  exit 1
fi

if grep -q "Syntax Error" tests/test_error.out; then
  echo "PASS: syntax error detected as expected"
  exit 0
else
  echo "FAIL: expected syntax error message not found"
  cat tests/test_error.out
  exit 1
fi
