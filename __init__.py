from .patchCrc.patchCrc import patch_vermagic, patch_crcs
from binaryninja import PluginCommand
from binaryninja.interaction import IntegerField, TextLineField, ChoiceField, get_form_input, SeparatorField, OpenFileNameField
from binaryninja import BinaryView
import argparse
import re

def patchCrc_p(bv):
    vmlinux_bndb_name = None
    ko_bndb_name = None
    formstr = 'patch driver for compatability with vmlinux'
    if bv.file.filename.startswith('vmlinux'):
        vmlinux_bndb_name = bv.file.filename
    else:
        ko_bndb_name = bv.file.filename
    bndb_vmlinux = OpenFileNameField("bndb of vmlinux", vmlinux_bndb_name)
    bndb_ko = OpenFileNameField("bndb of target driver", ko_bndb_name)

    input_list = [bndb_vmlinux, bndb_ko]
    get_form_input(input_list, formstr)

    # if the ko was our current view, use that ko
    if bndb_ko.result == bv.file.filename:
        bv_ko = bv
    else:
        bv_ko = BinaryView.open(bndb_ko.result)

    # if the vmlinux was our current view, use that
    if bndb_vmlinux.result == bv.file.filename:
        bv_vmlinux = bv
    else:
        bv_vmlinux = BinaryView.open(bndb_ko.result)

    patch_vermagic(bv_ko, bv_vmlinux)
    patch_crcs(bv_ko, bv_vmlinux)

    return

PluginCommand.register("patchCrc", "patch linux kernel driver", patchCrc_p)
