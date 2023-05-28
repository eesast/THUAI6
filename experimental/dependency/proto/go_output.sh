#!/usr/bin/env bash

set -e

PROTO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
pushd "${PROTO_DIR}"
buf generate
rm -rf ../../CAPI/go/API/proto
mv -f proto ../../CAPI/go/API
popd
