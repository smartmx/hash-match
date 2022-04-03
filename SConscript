from building import *
import rtconfig

cwd = GetCurrentDir()

# init src
src = []
src += Glob('*.c')

if GetDepend(['PKG_USING_HASH_MATCH_DEMO']):
    src += Glob("examples/hash_match_demo.c")

CPPPATH = [cwd]

group = DefineGroup('hash-match', src, depend = ['PKG_USING_HASH_MATCH'], CPPPATH = CPPPATH)

Return('group')
