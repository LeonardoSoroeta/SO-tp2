#!/bin/bash
qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 1024 -serial file:output.log -monitor stdio -soundhw pcspk -vga std
