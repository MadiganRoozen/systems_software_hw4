.text	0
a0:	SWR $sp, -1, $sp
a1:	SWR $sp, -2, $fp
a2:	SWR $sp, -3, $r3
a3:	SWR $sp, -4, $ra
a4:	CPR $fp, $sp
a5:	SRI $sp, 4
a6:	CPW $sp, 0, $gp, -1
a7:	PINT $sp, 0
a8:	CPW $sp, 0, $gp, -1
a9:	CPW $sp, 1, $gp, -1
a10:	MUL $sp, 1
a11:	CFLO $sp, 0
a12:	CPW $sp, 0, $gp, -1
a13:	CPW $sp, 1, $gp, -1
a14:	MUL $sp, 1
a15:	CFLO $sp, 0
a16:	CPW $sp, 0, $gp, -1
a17:	CPW $sp, 1, $gp, -1
a18:	MUL $sp, 1
a19:	CFLO $sp, 0
a20:	SUB $sp, 0, $sp, 1
a21:	SUB $sp, 0, $sp, 1
a22:	CPW $gp, -1, $sp, 0
a23:	CPW $sp, 0, $gp, -1
a24:	CPW $sp, 0, $gp, -1
a25:	CPW $sp, 1, $gp, -1
a26:	SUB $sp, 0, $sp, 1
a27:	SUB $sp, 0, $sp, 1
a28:	BLTZ $sp, 0, 18	# target is word address 46
a29:	CPW $sp, 0, $gp, -1
a30:	PINT $sp, 0
a31:	CPW $sp, 0, $gp, -1
a32:	CPW $sp, 0, $gp, -1
a33:	CPW $sp, 1, $gp, -1
a34:	SUB $sp, 0, $sp, 1
a35:	LIT $sp, 0, 0
a36:	LIT $sp, 1, 925
a37:	MUL $sp, 1
a38:	CFLO $sp, 0
a39:	LIT $sp, 1, -1778
a40:	MUL $sp, 1
a41:	CFLO $sp, 0
a42:	ADD $sp, 0, $sp, 1
a43:	SUB $sp, 0, $sp, 1
a44:	CPW $gp, -1, $sp, 0
a45:	JREL -22	# target is word address 23
a46:	CPW $sp, 0, $gp, -1
a47:	LIT $sp, 1, 0
a48:	SUB $sp, 0, $sp, 1
a49:	BLTZ $sp, 0, 10	# target is word address 59
a50:	CPW $sp, 0, $gp, -1
a51:	LIT $sp, 1, 0
a52:	BNE $sp, 1, 6	# target is word address 58
a53:	CPW $sp, 0, $gp, -1
a54:	CPW $sp, 1, $gp, -1
a55:	SUB $sp, 0, $sp, 1
a56:	PINT $sp, 0
a57:	JREL 1	# target is word address 58
a58:	JREL 10	# target is word address 68
a59:	CPW $sp, 0, $gp, -1
a60:	LIT $sp, 1, 0
a61:	SUB $sp, 0, $sp, 1
a62:	BGTZ $sp, 0, 4	# target is word address 66
a63:	LIT $sp, 0, 0
a64:	PINT $sp, 0
a65:	JREL 3	# target is word address 68
a66:	LIT $sp, 0, 0
a67:	PINT $sp, 0
a68:	LWR $ra, $fp, -4
a69:	LWR $r3, $fp, -1
a70:	LWR $fp, $fp, -2
a71:	CPR $sp, $r3
a72:	EXIT 0
.data	1028
.stack	6152
.end
