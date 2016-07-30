#!/usr/bin/python3

import colorsys
import numpy
import sys
import math

def gen_vals(hue,sat,lig):
	ret=[]
	for k in lig:
		ret.append(colorsys.hls_to_rgb(hue,k,1.0))
	return ret

def scale_val(v):
	ret=[]
	for k in v:
		ret.append(math.floor(255*k))
	return ret

def scale_vals(v):
	ret=[]
	for k in v:
		ret.append(scale_val(k))
	return ret

def write(name,f,v):
	for k in range(0,len(v)):
		print('%s %d:#%02x%02x%02x'%(name,k,v[k][0],v[k][1],v[k][2]),file=f)


huemap={}
huemap[0]=0
huemap[30]=30/360
huemap[60]=40/360
huemap[90]=50/360
huemap[120]=60/360
huemap[150]=80/360
huemap[180]=120/360
huemap[210]=180/360
huemap[240]=220/360
huemap[270]=250/360
huemap[300]=280/360
huemap[330]=330/360
huemap[360]=1

x=[]
f=[]
k=0
for hue in sorted(huemap):
	x.append(hue)
	f.append(huemap[hue])

huebase=180;
huedelta=30;
if len(sys.argv)>1:
	huebase=sys.argv[1]

if len(sys.argv)>2:
	huedeleta=30

hues=numpy.interp([huebase,(180+huebase+30)%360,(180+huebase-30)%360]\
	,x,f)
#l_poly=[0.668000,0.165333,0.083333]
#l_poly=[2/3,0,1/6]
l_poly=[0.501333,0.248667,0.083333]
x=numpy.linspace(0,1,5)
l=numpy.polyval(l_poly,x)

color_prim=scale_vals( gen_vals(hues[0],1.0,l) )
color_sec_a=scale_vals( gen_vals(hues[1],1.0,l) )
color_sec_b=scale_vals( gen_vals(hues[2],1.0,l) )

with open('colors.dict','w') as f:
	write('prim',f,color_prim)
	write('sec a',f,color_sec_a)
	write('sec b',f,color_sec_b)

