0x6a00
0x41a00
0x9ba00
0xa1a00
0xb6a00 --
0xbba00
0x105a00
0x16da00
0x180a00
0x19ca00
0x1bfa00
0x1c2a00
0x1d4a00
0x1e2a00
0x1eca00
0x1f5a00 --
0x2e8a00
0x2fea00
0x307a00
0x310a00
0x34fa00
0x35ea00
0x3c9a00
0x3cfa00
0x3dca00
0x44da00
0x451a00

0x4da90
0x6ca90
0xa4a90
0xb0a90
0xb6a90
0x106a90
0x120a90
0x163a90
0x1dfa90
0x1f5a90
0x21ea90
0x302a90
0x33da90
0x376a90
0x3a4a90
0x3afa90
0x3d2a90
0x446a90

0x1ead8
0x27ad8
0x5fad8
0x67ad8
0x83ad8
0xc9ad8
0xcead8
0xd3ad8
0xeead8
0x120ad8
0x123ad8
0x141ad8
0x157ad8
0x1bbad8
0x1e0ad8
0x257ad8
0x25fad8
0x264ad8
0x33aad8
0x33fad8
0x3a6ad8
0x3bcad8

    # for i in bv.functions:
    #     if (i.start & 0xfff) == 0xad8:
    #         print(hex(i.start))

def seeiffunc(addr):
	a = bv.find_all_data(bv.start, bv.end, addr.to_bytes(4, 'little'))
	if a.results.empty() == False:
		print("found with {} at {}".format(hex(addr), hex(a.results.get()[0])))
		return True
	return False
def ifcanfunc():
	for i in bv.functions:
		seeiffunc(i.start)

def findme12(addr):
	base = 0x007dc1bc
	for i in range(0x425f):
		symval = bv.read(base + i * 0x14 + 0x08, 4)
		symval = int.from_bytes(symval, 'little')
		if (addr & 0xfff) == (symval & 0xfff):
			print("found {}, index {} address {}".format(addr, i, base + i * 0x14))
		
def findmename():
	base = 0x007dc1bc
	min = 0
	minindex = 0
	for i in range(0x425f):
		symval = bv.read(base + i * 0x14 + 0x04, 4)
		symval = int.from_bytes(symval, 'big')
		if min == 0:
			min = symval
			minindex = i
		if min > symval:
			print("{} > {}, at {}".format(hex(min), hex(symval), hex(base + i * 0x14)))
			min = symval
			minindex = i
	print("smallest strname is {}, index {}, address {}".format(hex(min), minindex, hex(base + minindex * 0x14)))

def vxworksnameall(base, symtabsz):
	# base = 0x007ec1bc
	from ctypes import c_char_p as c_char_p
	for i in range(symtabsz):
		symval = bv.read(base + i * 0x14 + 0x04, 4)
		symname = int.from_bytes(symval, 'big')
		symnameraw = bv.read(symname, 0x40)
		symnameraw = (c_char_p(symnameraw).value)
		symnameraw = symnameraw.decode('utf-8')
		symval = bv.read(base + i * 0x14 + 0x08, 4)
		symval = int.from_bytes(symval, 'big')
		assumedtype = bv.read(base + i * 0x14 + 0x10, 4)
		assumedtype = int.from_bytes(assumedtype, 'big')
		targsym = bv.get_symbol_at(symval)
		if targsym != None:
			newsym = types.Symbol(targsym.type, targsym.address, symnameraw)
			bv.define_user_symbol(newsym)
		else:
			if assumedtype == 0x500:
				localtype = SymbolType.FunctionSymbol
			elif assumedtype == 0x900:
				localtype = SymbolType.DataSymbol
			elif assumedtype == 0x1100:
				localtype = SymbolType.ExternalSymbol
			elif assumedtype == 0x300:
				print("constant found, continuaing")
				continue
			else:
				print("foudn type {} at address {}".format(hex(assumedtype), hex(base + i * 0x14)))
				break
			newsym = types.Symbol(localtype, symval, symnameraw)
			bv.define_user_symbol(newsym)


VxWorksSymTab_Finder.java> Searching for symbol table variant 0
   block: ram (00000000, 0082f42f) testBlkSize = 7d0  .......
VxWorksSymTab_Finder.java> Symbol table at 007eb784 (173 entries)
VxWorksSymTab_Finder.java> Warning: Symbol Table Size not found before of after table
VxWorksSymTab_Finder.java> Creating SYMBOL data type and symbol table structure...
VxWorksSymTab_Finder.java> Processing symbol table entries.
VxWorksSymTab_Finder.java> i=0, nameAddr: 0065b7a0, loc: 00322a74, type: 5, name: _lookup_root
VxWorksSymTab_Finder.java> i=1, nameAddr: 0065b7c0, loc: 00323004, type: 5, name: ot
VxWorksSymTab_Finder.java> i=2, nameAddr: 0065b7e0, loc: 00322a64, type: 5, name: t
VxWorksSymTab_Finder.java> i=3, nameAddr: 0065b7f8, loc: 00322a6c, type: 5, name: fprintf
VxWorksSymTab_Finder.java> i=4, nameAddr: 0065b818, loc: 00322fcc, type: 5, name: telist_alloc_cleanup
VxWorksSymTab_Finder.java> i=5, nameAddr: 0065b838, loc: 00322f98, type: 5, name: telist_comm_point
VxWorksSymTab_Finder.java> i=6, nameAddr: 0065b858, loc: 003229c4, type: 5, name: telist_comm_signal
VxWorksSymTab_Finder.java> i=7, nameAddr: 0065b878, loc: 003229ac, type: 5, name: telist_comm_timer
VxWorksSymTab_Finder.java> i=8, nameAddr: 0065b898, loc: 00322fa0, type: 5, name: telist_event
VxWorksSymTab_Finder.java> i=9, nameAddr: 0065b8b8, loc: 00322b98, type: 5, name: ash_compfunc
VxWorksSymTab_Finder.java> i=10, nameAddr: 0065b8d8, loc: 00322fa8, type: 5, name: ash_deldatafunc
VxWorksSymTab_Finder.java> i=11, nameAddr: 0065b8f0, loc: 00322b88, type: 5, name: telist_hash_delkeyfunc
VxWorksSymTab_Finder.java> i=12, nameAddr: 0065b910, loc: 00322fb4, type: 5, name: telist_hash_markdelfunc
VxWorksSymTab_Finder.java> i=13, nameAddr: 0065b930, loc: 00322e98, type: 5, name: telist_hash_sizefunc
VxWorksSymTab_Finder.java> i=14, nameAddr: 0065b950, loc: 00324640, type: 5, name: telist_mesh_cb
VxWorksSymTab_Finder.java> i=15, nameAddr: 0065b968, loc: 00324528, type: 5, name: telist_mod_clear
VxWorksSymTab_Finder.java> i=16, nameAddr: 0065b980, loc: 00324184, type: 5, name: fptr_whitelist_mod_deinit
VxWorksSymTab_Finder.java> i=17, nameAddr: 0065b998, loc: 003240ec, type: 5, name: t
VxWorksSymTab_Finder.java> i=18, nameAddr: 0065b9a8, loc: 00324354, type: 5, name: telist_mod_get_mem
VxWorksSymTab_Finder.java> i=19, nameAddr: 0065b9c8, loc: 00463918, type: 5, name: telist_mod_inform_super
VxWorksSymTab_Finder.java> i=20, nameAddr: 0065b9e8, loc: 00323fd0, type: 5, name: telist_mod_init
VxWorksSymTab_Finder.java> i=21, nameAddr: 0065b9f8, loc: 00324948, type: 5, name: fptr_whitelist_mod_operate
VxWorksSymTab_Finder.java> i=22, nameAddr: 0065ba10, loc: 00463888, type: 5, name: te
VxWorksSymTab_Finder.java> i=23, nameAddr: 0065ba30, loc: 0032491c, type: 5, name: tach_sub
VxWorksSymTab_Finder.java> i=24, nameAddr: 0065ba48, loc: 00324914, type: 5, name: telist_modenv_detach_subs
VxWorksSymTab_Finder.java> i=25, nameAddr: 0065ba60, loc: 00324938, type: 5, name: s
VxWorksSymTab_Finder.java> i=26, nameAddr: 0065ba78, loc: 00324940, type: 5, name: odenv_detect_cycle
VxWorksSymTab_Finder.java> i=27, nameAddr: 0065ba90, loc: 003256b0, type: 5, name: fptr_whitelist_modenv_kill_sub
VxWorksSymTab_Finder.java> i=28, nameAddr: 0065baa0, loc: 00325884, type: 5, name: odenv_kill_sub
VxWorksSymTab_Finder.java> i=29, nameAddr: 0065bab8, loc: 00325540, type: 5, name: telist_modenv_send_query
VxWorksSymTab_Finder.java> i=30, nameAddr: 0065bac8, loc: 003258dc, type: 5, name: nd_query
VxWorksSymTab_Finder.java> i=31, nameAddr: 0065bae0, loc: 00325580, type: 5, name: telist_pending_tcp
VxWorksSymTab_Finder.java> i=32, nameAddr: 0065baf8, loc: 0032550c, type: 5, name: fptr_whitelist_pending_udp
VxWorksSymTab_Finder.java> i=33, nameAddr: 0065bb10, loc: 003258d4, type: 5, name: dp
VxWorksSymTab_Finder.java> i=34, nameAddr: 0065bb28, loc: 003258e4, type: 5, name: rint_func
VxWorksSymTab_Finder.java> i=35, nameAddr: 0065bb40, loc: 00325654, type: 5, name: telist_rbtree_cmp
VxWorksSymTab_Finder.java> i=36, nameAddr: 0065bb50, loc: 003254f4, type: 5, name: p
VxWorksSymTab_Finder.java> i=37, nameAddr: 0065bb68, loc: 00359254, type: 5, name: erviced_query
VxWorksSymTab_Finder.java> createFunction: Failed to create function
VxWorksSymTab_Finder.java> i=38, nameAddr: 0065bb78, loc: 0063f628, type: 9, name: fptr_whitelist_start_accept
VxWorksSymTab_Finder.java> i=39, nameAddr: 0065bb88, loc: 00359248, type: 5, name: tart_accept
VxWorksSymTab_Finder.java> createFunction: Failed to create function
VxWorksSymTab_Finder.java> i=40, nameAddr: 0065bb98, loc: 00325bb4, type: 5, name: fptr_whitelist_stop_accept
VxWorksSymTab_Finder.java> i=41, nameAddr: 0065bbb8, loc: 00325e94, type: 5, name: fputc
VxWorksSymTab_Finder.java> i=42, nameAddr: 0065bbd8, loc: 00325b68, type: 5, name: freeLeaseByIndex
VxWorksSymTab_Finder.java> i=43, nameAddr: 0065bbf8, loc: 00325ae4, type: 5, name: ex
VxWorksSymTab_Finder.java> i=44, nameAddr: 0065bc10, loc: 00325e6c, type: 5, name: free_sys_varlist
VxWorksSymTab_Finder.java> i=45, nameAddr: 0065bc30, loc: 00325b80, type: 5, name: list
VxWorksSymTab_Finder.java> i=46, nameAddr: 0065bc50, loc: 00325cd4, type: 5, name: buf
VxWorksSymTab_Finder.java> i=47, nameAddr: 0065bc70, loc: 00325f2c, type: 5, name: ong
VxWorksSymTab_Finder.java> i=48, nameAddr: 0065bc90, loc: 00325aa4, type: 5, name: tilLibInit
VxWorksSymTab_Finder.java> i=49, nameAddr: 0065bca8, loc: 00325e44, type: 5, name: rInit
VxWorksSymTab_Finder.java> i=50, nameAddr: 0065bcc0, loc: 00325b04, type: 5, name: Raise
VxWorksSymTab_Finder.java> i=51, nameAddr: 0065bce0, loc: 00325c00, type: 5, name: fsWaitForPath
VxWorksSymTab_Finder.java> i=52, nameAddr: 0065bd00, loc: 00325b98, type: 5, name: fsetpos
VxWorksSymTab_Finder.java> i=53, nameAddr: 0065bd20, loc: 00325b48, type: 5, name: fsmGetVolume
VxWorksSymTab_Finder.java> i=54, nameAddr: 0065bd40, loc: 00325b88, type: 5, name: fsmNameInstall
VxWorksSymTab_Finder.java> i=55, nameAddr: 0065bd60, loc: 00325ba0, type: 5, name: fsmNameUninstall
VxWorksSymTab_Finder.java> i=56, nameAddr: 0065bd80, loc: 00325b90, type: 5, name: Install
VxWorksSymTab_Finder.java> i=57, nameAddr: 0065bda0, loc: 00325fa8, type: 5, name: fsmUnmountHookAdd
VxWorksSymTab_Finder.java> i=58, nameAddr: 0065bdc0, loc: 00323c30, type: 5, name: ntHookDelete
VxWorksSymTab_Finder.java> i=59, nameAddr: 0065bdd8, loc: 003178e0, type: 5, name: ntHookRun
VxWorksSymTab_Finder.java> i=60, nameAddr: 0065bdf0, loc: 00317880, type: 5, name: fstatfs
VxWorksSymTab_Finder.java> i=61, nameAddr: 0065be08, loc: 00323c00, type: 5, name: ftpCommand
VxWorksSymTab_Finder.java> i=62, nameAddr: 0065be18, loc: 003177f8, type: 5, name: ftpCommandEnhanced
VxWorksSymTab_Finder.java> i=63, nameAddr: 0065be28, loc: 003176c0, type: 5, name: ed
VxWorksSymTab_Finder.java> i=64, nameAddr: 0065be38, loc: 00317254, type: 5, name: onnGet
VxWorksSymTab_Finder.java> i=65, nameAddr: 0065be48, loc: 004639e0, type: 5, name: onnInit
VxWorksSymTab_Finder.java> i=66, nameAddr: 0065be60, loc: 00327f58, type: 5, name: assiveMode
VxWorksSymTab_Finder.java> i=67, nameAddr: 0065be70, loc: 003282d0, type: 5, name: ftpHookup
VxWorksSymTab_Finder.java> i=68, nameAddr: 0065be88, loc: 00320c18, type: 5, name: bugOptionsSet
VxWorksSymTab_Finder.java> i=69, nameAddr: 0065bea0, loc: 00327e4c, type: 5, name: it
VxWorksSymTab_Finder.java> i=70, nameAddr: 0065beb8, loc: 003255e0, type: 5, name: ftpLs
VxWorksSymTab_Finder.java> i=71, nameAddr: 0065bed0, loc: 003205b4, type: 5, name: ftpReplyGetEnhanced
VxWorksSymTab_Finder.java> i=72, nameAddr: 0065bee0, loc: 00321fdc, type: 5, name: ced
VxWorksSymTab_Finder.java> i=73, nameAddr: 0065bef8, loc: 00321654, type: 5, name: ftpTransientConfigGet
VxWorksSymTab_Finder.java> i=74, nameAddr: 0065bf08, loc: 00323bf4, type: 5, name: igGet
VxWorksSymTab_Finder.java> i=75, nameAddr: 0065bf18, loc: 00324eb8, type: 5, name: ientConfigSet
VxWorksSymTab_Finder.java> i=76, nameAddr: 0065bf28, loc: 00317244, type: 5, name: ftpTransientFatal
VxWorksSymTab_Finder.java> i=77, nameAddr: 0065bf38, loc: 0032d8a8, type: 5, name: l
VxWorksSymTab_Finder.java> i=78, nameAddr: 0065bf48, loc: 00320af4, type: 5, name: ientFatalInstall
VxWorksSymTab_Finder.java> i=79, nameAddr: 0065bf68, loc: 00320b74, type: 5, name: se
VxWorksSymTab_Finder.java> i=80, nameAddr: 0065bf80, loc: 00317238, type: 5, name: g
VxWorksSymTab_Finder.java> i=81, nameAddr: 0065bf90, loc: 00320bfc, type: 5, name: ModeDisable
VxWorksSymTab_Finder.java> i=82, nameAddr: 0065bfa8, loc: 0032daa4, type: 5, name: sientMaxRetryCount
VxWorksSymTab_Finder.java> i=83, nameAddr: 0065bfb8, loc: 0032daf4, type: 5, name: nt
VxWorksSymTab_Finder.java> i=84, nameAddr: 0065bfc8, loc: 00320d08, type: 5, name: sientRetryInterval
VxWorksSymTab_Finder.java> i=85, nameAddr: 0065bfe0, loc: 0035c61c, type: 5, name: full_recvbuffs
VxWorksSymTab_Finder.java> i=86, nameAddr: 0065c000, loc: 0035c628, type: 5, name: P
VxWorksSymTab_Finder.java> i=87, nameAddr: 0065c020, loc: 0035c640, type: 5, name: fwGroupPredefinedMAC
VxWorksSymTab_Finder.java> i=88, nameAddr: 0065c040, loc: 0035c634, type: 5, name: fwrite
VxWorksSymTab_Finder.java> i=89, nameAddr: 0065c060, loc: 0035c64c, type: 5, name: _create
VxWorksSymTab_Finder.java> i=90, nameAddr: 0065c080, loc: 0035c610, type: 5, name: fwtpl_id_find
VxWorksSymTab_Finder.java> i=91, nameAddr: 0065c0a0, loc: 003172a8, type: 5, name: _name
VxWorksSymTab_Finder.java> i=92, nameAddr: 0065c0b8, loc: 00317260, type: 5, name: fwtpl_init
VxWorksSymTab_Finder.java> i=93, nameAddr: 0065c0d0, loc: 00323be8, type: 5, name: _id_table
VxWorksSymTab_Finder.java> i=94, nameAddr: 0065c0e0, loc: 00323bc4, type: 5, name: fwtpl_net_activate
VxWorksSymTab_Finder.java> i=95, nameAddr: 0065c0f8, loc: 00323bd0, type: 5, name: fwtpl_net_create
VxWorksSymTab_Finder.java> i=96, nameAddr: 0065c110, loc: 00323bdc, type: 5, name: fwtpl_net_deactivate
VxWorksSymTab_Finder.java> i=97, nameAddr: 0065c130, loc: 00323bb8, type: 5, name: t_destroy
VxWorksSymTab_Finder.java> i=98, nameAddr: 0065c148, loc: 00323c24, type: 5, name: t_find
VxWorksSymTab_Finder.java> i=99, nameAddr: 0065c160, loc: 00320590, type: 5, name: y_name
VxWorksSymTab_Finder.java> i=100, nameAddr: 0065c178, loc: 00320bd8, type: 5, name: g_ipssh
VxWorksSymTab_Finder.java> i=101, nameAddr: 0065c188, loc: 00321fe4, type: 5, name: FunctionList
VxWorksSymTab_Finder.java> i=102, nameAddr: 0065c1a8, loc: 003205a8, type: 5, name: gen_md5
VxWorksSymTab_Finder.java> i=103, nameAddr: 0065c1c0, loc: 0032059c, type: 5, name: callback
VxWorksSymTab_Finder.java> i=104, nameAddr: 0065c1d8, loc: 0032164c, type: 5, name: HmImageHeaderResultString
VxWorksSymTab_Finder.java> i=105, nameAddr: 0065c1e8, loc: 00320a00, type: 5, name: ultString
VxWorksSymTab_Finder.java> createFunction: Failed to create function
VxWorksSymTab_Finder.java> i=106, nameAddr: 0065c208, loc: 003204c4, type: 5, name: getCreationDateOnly
VxWorksSymTab_Finder.java> i=107, nameAddr: 0065c228, loc: 003209ec, type: 5, name: ease
VxWorksSymTab_Finder.java> i=108, nameAddr: 0065c240, loc: 00320ae8, type: 5, name: t
VxWorksSymTab_Finder.java> i=109, nameAddr: 0065c258, loc: 0063af58, type: 9, name: getOptServ
VxWorksSymTab_Finder.java> i=110, nameAddr: 0065c260, loc: 0031783c, type: 5, name: rv
VxWorksSymTab_Finder.java> i=111, nameAddr: 0065c270, loc: 0031775c, type: 5, name: seString
VxWorksSymTab_Finder.java> i=112, nameAddr: 0065c280, loc: 00463ddc, type: 5, name: getSwType
VxWorksSymTab_Finder.java> i=113, nameAddr: 0065c298, loc: 00463acc, type: 5, name: mes
VxWorksSymTab_Finder.java> i=114, nameAddr: 0065c2b0, loc: 00463c54, type: 5, name: getUInt32
VxWorksSymTab_Finder.java> i=115, nameAddr: 0065c2c8, loc: 0031722c, type: 5, name: d
VxWorksSymTab_Finder.java> i=116, nameAddr: 0065c2d8, loc: 003162d4, type: 5, name: AndIncr_async
VxWorksSymTab_Finder.java> i=117, nameAddr: 0065c2f0, loc: 00326578, type: 5, name: rom_version_internal
VxWorksSymTab_Finder.java> i=118, nameAddr: 0065c308, loc: 00328528, type: 5, name: get_cname_target
VxWorksSymTab_Finder.java> i=119, nameAddr: 0065c328, loc: 003284dc, type: 5, name: _recv_buffer
VxWorksSymTab_Finder.java> i=120, nameAddr: 0065c348, loc: 0032858c, type: 5, name: ffer
VxWorksSymTab_Finder.java> i=121, nameAddr: 0065c368, loc: 0032855c, type: 5, name: athname
VxWorksSymTab_Finder.java> i=122, nameAddr: 0065c388, loc: 00328574, type: 5, name: EngineBoots
VxWorksSymTab_Finder.java> i=123, nameAddr: 0065c3b0, loc: 003264a8, type: 5, name: get_snmpEngineTime
VxWorksSymTab_Finder.java> i=124, nameAddr: 0065c3c8, loc: 00328474, type: 5, name: get_systime
VxWorksSymTab_Finder.java> i=125, nameAddr: 0065c3e8, loc: 003284a4, type: 5, name: getaddrinfo
VxWorksSymTab_Finder.java> i=126, nameAddr: 0065c408, loc: 003284b8, type: 5, name: getbyte
VxWorksSymTab_Finder.java> i=127, nameAddr: 0065c430, loc: 00328460, type: 5, name: NonBlocking
VxWorksSymTab_Finder.java> i=128, nameAddr: 0065c450, loc: 00316ef4, type: 5, name: on
VxWorksSymTab_Finder.java> i=129, nameAddr: 0065c460, loc: 00316968, type: 5, name: getconfig
VxWorksSymTab_Finder.java> i=130, nameAddr: 0065c470, loc: 00316f00, type: 5, name: getcwd
VxWorksSymTab_Finder.java> i=131, nameAddr: 0065c488, loc: 00316f90, type: 5, name: yaddr
VxWorksSymTab_Finder.java> createFunction: Failed to create function
VxWorksSymTab_Finder.java> i=132, nameAddr: 0065c498, loc: 00324e60, type: 5, name: yname
VxWorksSymTab_Finder.java> i=133, nameAddr: 0065c4b0, loc: 00317988, type: 5, name: getinterface
VxWorksSymTab_Finder.java> i=134, nameAddr: 0065c4c8, loc: 003172a0, type: 5, name: byaddr
VxWorksSymTab_Finder.java> i=135, nameAddr: 0065c4e0, loc: 003230ec, type: 5, name: getnameinfo
VxWorksSymTab_Finder.java> i=136, nameAddr: 0065c4f8, loc: 003231a0, type: 5, name: m
VxWorksSymTab_Finder.java> i=137, nameAddr: 0065c510, loc: 00323134, type: 5, name: it
VxWorksSymTab_Finder.java> i=138, nameAddr: 0065c528, loc: 0032320c, type: 5, name: getpass
VxWorksSymTab_Finder.java> i=139, nameAddr: 0065c538, loc: 00323094, type: 5, name: ame
VxWorksSymTab_Finder.java> i=140, nameAddr: 0065c550, loc: 00323110, type: 5, name: error
VxWorksSymTab_Finder.java> i=141, nameAddr: 0065c568, loc: 00323038, type: 5, name: getproc_got_empty
VxWorksSymTab_Finder.java> i=142, nameAddr: 0065c580, loc: 00324a40, type: 5, name: getproc_got_int32
VxWorksSymTab_Finder.java> i=143, nameAddr: 0065c590, loc: 003161c8, type: 5, name: 2
VxWorksSymTab_Finder.java> i=144, nameAddr: 0065c5a8, loc: 003209d8, type: 5, name: ddress
VxWorksSymTab_Finder.java> i=145, nameAddr: 0065c5c0, loc: 00320adc, type: 5, name: ct_id
VxWorksSymTab_Finder.java> i=146, nameAddr: 0065c5d8, loc: 0032797c, type: 5, name: t_string
VxWorksSymTab_Finder.java> createFunction: Failed to create function
VxWorksSymTab_Finder.java> i=147, nameAddr: 0065c5f8, loc: 003215cc, type: 5, name: ng
VxWorksSymTab_Finder.java> i=148, nameAddr: 0065c608, loc: 0035c658, type: 5, name: got_uint32
VxWorksSymTab_Finder.java> i=149, nameAddr: 0065c620, loc: 00317940, type: 5, name: got_uint64
VxWorksSymTab_Finder.java> createFunction: Failed to create function
VxWorksSymTab_Finder.java> i=150, nameAddr: 0065c638, loc: 00324950, type: 5, name: got_uint64_high_low
VxWorksSymTab_Finder.java> i=151, nameAddr: 0065c648, loc: 00322118, type: 5, name: low
VxWorksSymTab_Finder.java> createFunction: Failed to create function
VxWorksSymTab_Finder.java> i=152, nameAddr: 0065c660, loc: 003251b8, type: 5, name: s
VxWorksSymTab_Finder.java> i=153, nameAddr: 0065c680, loc: 0063b428, type: 9, name: getsInputsToInt
VxWorksSymTab_Finder.java> i=154, nameAddr: 0065c690, loc: 0035d2d8, type: 5, name: getsV24
VxWorksSymTab_Finder.java> i=155, nameAddr: 0065c6a8, loc: 0035d7e0, type: 5, name: n
VxWorksSymTab_Finder.java> i=156, nameAddr: 0065c6c0, loc: 0035d834, type: 5, name: getservbyport
VxWorksSymTab_Finder.java> i=157, nameAddr: 0065c6d8, loc: 0035d700, type: 5, name: ame
VxWorksSymTab_Finder.java> i=158, nameAddr: 0065c6f0, loc: 0035d298, type: 5, name: getspeed
VxWorksSymTab_Finder.java> i=159, nameAddr: 0065c708, loc: 0035d954, type: 5, name: up
VxWorksSymTab_Finder.java> i=160, nameAddr: 0065c720, loc: 0035d258, type: 5, name: gettype
VxWorksSymTab_Finder.java> i=161, nameAddr: 0065c730, loc: 0035d21c, type: 5, name: getwd
VxWorksSymTab_Finder.java> i=162, nameAddr: 0065c748, loc: 0035d1a4, type: 5, name: gmtime
VxWorksSymTab_Finder.java> i=163, nameAddr: 0065c760, loc: 0035d198, type: 5, name: gpGlobalBus
VxWorksSymTab_Finder.java> i=164, nameAddr: 0065c778, loc: 0035ce40, type: 5, name: _getproc_and_instance
VxWorksSymTab_Finder.java> i=165, nameAddr: 0065c788, loc: 0035ce00, type: 5, name: tance
VxWorksSymTab_Finder.java> i=166, nameAddr: 0065c7a0, loc: 0035cdc0, type: 5, name: grouptable_next
VxWorksSymTab_Finder.java> i=167, nameAddr: 0065c7b0, loc: 0035cd04, type: 5, name: grouptable_set
VxWorksSymTab_Finder.java> i=168, nameAddr: 0065c7c8, loc: 0035cc8c, type: 5, name: le_test
VxWorksSymTab_Finder.java> i=169, nameAddr: 0065c7e0, loc: 0035cd40, type: 5, name: h_errno
VxWorksSymTab_Finder.java> i=170, nameAddr: 0065c800, loc: 0035cc80, type: 5, name: ntextGet
VxWorksSymTab_Finder.java> i=171, nameAddr: 0065c818, loc: 00b944e0, type: 17, name: ntextGetBase
VxWorksSymTab_Finder.java> i=172, nameAddr: 0065c828, loc: 0063f86c, type: 9, name: handleContextSet
VxWorksSymTab_Finder.java> Finished!
