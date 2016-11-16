#!/bin/bash
qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -net nic,model=rtl8139 -m 512 
