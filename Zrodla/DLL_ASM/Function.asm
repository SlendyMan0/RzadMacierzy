; GENERAL PURPOSE REGISTERS:
;
; legacy registers: RAX, RBX, RCX, RDX, RBP, RSI, RDI, RSP
; to get to 32 bit part (DWORD) use E instead of R
; to get to 16 bit part (WORD), remove R altogether
; to get to 8 bit low part (BYTE), remove R and replace X with L
; to get to 8 bit high part (BYTE), remove R and replace X with H
;
; new registers   : R8 , R9 , R10, R11, R12, R13, R14, R15
; to get to 32 bit part (DWORD) add D
; to get to 16 bit part (WORD), add W
; to get to 8 bit low part (BYTE), add B
; you can't get to 8 bit high part
;
; PASSED ARGUMENTS:
;
; 1 parameter: RCX
; 2 parameter: RDX
; 3 parameter: R8
; 4 parameter: R9
; rest in stack




.code
rank proc
	; EDX has deleteTemplateColumn
	; R8D has deleteTemplateRow
	; R12D has counterColumn
	; R15D has pointer to matrix (also [matrix] is the size)

	mov r15d, ecx													; moving pointer to matrix out of the way
	mov ecx, dword ptr [r15d]										; moving the matrix size n to ecx to set up the loop
	xor r12d, r12d													; clearing R12D for use
l1: 
	mov eax, edx													; copying deleteTemplateColumn
	dec ecx															; getting better value of loop (as I use 0 too)
	shr eax, cl														; shifing deleteTemplateColumn to compare bits
	and eax, 1														; comparing bits
	add r12d, eax													; if bit was 1 it is added to the counter
	inc ecx															; original value of loop counter
	loop l1															; jump to l1, decrease 1 from loop counter

	mov eax, dword ptr [r15d]										; starting (size - counterColumn) == 1 check
	sub eax, r12d
	cmp eax, 1														; is it 1x1 matrix?
	jne j1															; if not 1x1 matrix, jump
	mov ecx, dword ptr [r15d]										; moving the matrix size n to ecx to set up the loop
l2: 
	mov eax, edx													; copying deleteTemplateColumn
	dec ecx															; getting better value of loop (as I use 0 too)
	shr eax, cl														; shifing deleteTemplateColumn to compare bits
	and eax, 1														; comparing bits
	cmp eax, 0														; checking if it is the only not excluded column
	je jcf															; jumping to set the number correctly (jump column found)
jbc:
	inc ecx															; original value of loop counter

	mov eax, r8d													; copying deleteTemplateRow
	dec ecx															; getting better value of loop (as I use 0 too)
	shr eax, cl														; shifing deleteTemplateColumn to compare bits
	and eax, 1														; comparing bits
	cmp eax, 0														; checking if it is the only not excluded row
	je jrf															; jumping to set the number correctly (jump row found)
jbr:
	inc ecx															; original value of loop counter
	loop l2	

	mov ecx, dword ptr [r15d]										; preparing to calculate the exact position of number to return 
	imul ecx, r10d													; multiplying by row index
	add ecx, r9d													; adding column index
	inc ecx															; remembering that first position in matrix is it's size
	imul ecx, 4														; remembering that dword is 4 bytes
	add ecx, r15d													; calculating pointer

	mov eax, dword ptr [ecx]										; returning value
	ret
jcf:
	mov r9d, ecx													; saving column index
	jmp jbc															; jumping back (jump back column)
jrf:
	mov r10d, ecx													; saving row index
	jmp jbr															; jumping back (jump back row)
j1:
	mov eax, -1
	ret
rank endp

end