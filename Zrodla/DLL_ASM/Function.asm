.data

pointerMatrix dword ?
sizeMatrix dword ?

.code
determinant proc
	LOCAL deleteTemplateRow: dword, deleteTemplateColumn: dword, detValue: sdword

	mov detValue, 0
	mov deleteTemplateColumn, edx									; saving deleteTemplateColumn to memory
	mov deleteTemplateRow, r8d										; saving deleteTemplateRow to memory

	xor r10d, r10d													; clearing R10D for use
	xor ecx, ecx													; loop counter to zero
l1:	
	mov eax, edx													; copying deleteTemplateColumn
	shr eax, cl														; shifing deleteTemplateColumn to compare bits
	and eax, 1														; comparing bits
	add r10d, eax													; if bit was 1 it is added to the counter
	inc ecx															; i++
	cmp ecx, sizeMatrix												; is i=size
	jne l1

	mov eax, sizeMatrix												; starting (size - counterColumn) == 1 check
	sub eax, r10d													; size - counterColumn
	cmp eax, 1														; is it 1x1 matrix?
	jne j1															; if not 1x1 matrix, jump
	xor ecx, ecx													; loop counter to zero
	xor r9d, r9d
	xor r10d, r10d

	movd mm0, edx
	movd mm1, r8d
	punpckldq mm0, mm1
l2:
	movd mm6, ecx
	movq mm1, mm0
	psrld mm1, mm6
	pandn mm1, mm7

	punpckldq mm6, mm6
	pmullw mm1, mm6

	movd eax, mm1
	punpckhdq mm1, mm1
	movd edx, mm1

	add r9d, eax
	add r10d, edx													; the index must be subtracted from size of the matrix and further decreased by 1

	inc ecx															; i++
	cmp ecx, sizeMatrix												; is i=size
	jne l2	

	mov ecx, sizeMatrix												; preparing to calculate the exact position of number to return
	mov edx, ecx													
	sub edx, r10d
	dec edx 
	imul ecx, edx													; multiplying by now correct row index
	add ecx, r9d													; adding column index
	inc ecx															; remembering that first position in matrix is it's size
	shl ecx, 2														; remembering that dword is 4 bytes
	add ecx, pointerMatrix											; calculating pointer

	mov eax, dword ptr [ecx]										; returning value
	ret
j1:																	
	xor edx, edx													; loop counter to zero
l3:																	; finding first non-excluded row
	mov eax, deleteTemplateRow										; copying deleteTemplateRow
	mov ecx, sizeMatrix
	sub ecx, edx
	dec ecx
	shr eax, cl														; shifing deleteTemplateRow to compare bits
	inc edx															; i++
	and eax, 1														; comparing bits
	jnz l3															; jump if excluded

	dec edx															; i++ was not needed, non-excluded row found
	mov r9d, edx													; saving firstEmptyRow
	xor ecx, ecx													; loop counter to zero
	xor r10d, r10d													; clearing k counter
l4:
	mov eax, deleteTemplateColumn									; copying deleteTemplateColumn
	shr eax, cl														; shifing deleteTemplateColumn to compare bits
	and eax, 1														; comparing bits
	jnz skip
	inc r10d														; k++

	mov eax, 1														; pow(-1, (1 + k))
	add eax, r10d													; + k
	and eax, 1														; checking the paritiy
	push rax														; saving pairity check for later													
	mov eax, sizeMatrix												; setting up access for matrix[(size*firstEmptyRow) + i + 1]
	imul eax, r9d													; (size*firstEmptyRow)
	add eax, ecx													; + i
	inc eax															; + 1
	shl eax, 2														; remembering that dword is 4 bytes
	add eax, pointerMatrix											; calculating pointer
	
	mov r11d, dword ptr [eax]										; matrix[(size*firstEmptyRow) + i + 1]

	mov eax, deleteTemplateColumn									; saving data on stack			
	push rax
	mov eax, deleteTemplateRow						
	push rax
	mov eax, detValue						
	push rax
	mov detValue, 0
	push r10
	push r9
	push rcx
	push r11

	mov edx, 1														; setting second parameter
	shl edx, cl														; pow(2, i)
	add edx, deleteTemplateColumn									; + deleteTemplateColumn
	mov r8d, 1														; setting third parameter
	mov ecx, sizeMatrix
	sub ecx, r9d
	dec ecx
	shl r8d, cl														; pow(2, firstEmptyRow)
	add r8d, deleteTemplateRow										; + deleteTemplateRow
	mov ecx, pointerMatrix											; setting first parameter

	call determinant												; determinant(matrix, deleteTemplateColumn + pow(2, i), deleteTemplateRow + pow(2, firstEmptyRow))

	pop r11
	imul r11d, eax													; final multiplying
	pop rcx															; resetting values
	pop r9
	pop r10
	pop rax
	mov detValue, eax
	pop rax
	mov deleteTemplateRow, eax
	pop rax
	mov deleteTemplateColumn, eax	

	xor edx, edx													; clearing register
	pop rax
	cmp al, 0
	je aDet
	sub detValue, r11d
	jmp skip
aDet:
	add detValue, r11d
skip:
	inc ecx															; i++
	cmp ecx, sizeMatrix												; i=size?
	jne l4

	mov eax, detValue 

	ret
determinant endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


rank proc
	LOCAL deleteTemplateRow2: dword, deleteTemplateColumn2: dword, rankValue: sdword

	mov rankValue, 0
	mov pointerMatrix, ecx											; saving pointer to matrix to memory
	mov eax, dword ptr [ecx]
	mov sizeMatrix, eax												; saving size of matrix to memory
	mov deleteTemplateColumn2, edx									; saving deleteTemplateColumn to memory
	mov deleteTemplateRow2, r8d										; saving deleteTemplateRow to memory

	xor ecx, ecx													; loop counter to zero

	mov eax, 1
	movd mm7, eax
	punpckldq mm7, mm7
	movd mm0, edx
	movd mm1, r8d
	punpckldq mm0, mm1
	pxor mm2, mm2

pl1:
	movd mm6, ecx
	movq mm1, mm0
	psrld mm1, mm6
	pand mm1, mm7
	paddd mm2, mm1
	inc ecx
	cmp ecx, sizeMatrix
	jne pl1

	movd r10d, mm2
	punpckhdq mm2, mm2
	movd r11d, mm2

	cmp r10d, r11d													; (counterColumn != counterRow)
	je mif1
	mov eax, -1
	ret

mif1:
	cmp r10d, 0
	jne mif2

	mov ecx, pointerMatrix
	mov edx, deleteTemplateColumn2
	mov r8d, deleteTemplateRow2
	call determinant

	cmp eax, 0
	jne fr1
	mov eax, -2
	ret
fr1:
	mov eax, sizeMatrix
	ret

mif2:
	push r10
	mov ecx, pointerMatrix
	mov edx, deleteTemplateColumn2
	mov r8d, deleteTemplateRow2	
	call determinant
	pop r10

	cmp eax, 0
	jne fif1
	mov eax, sizeMatrix
	dec eax
	cmp eax, r10d
	je fif1
	
	xor r8d, r8d
	xor r9d, r9d
fl1:
	mov eax, deleteTemplateColumn2	
	mov ecx, r8d
	shr eax, cl	
	and eax, 1
	jnz fle1

fl2:
	mov eax, deleteTemplateRow2	
	mov ecx, r9d
	shr eax, cl	
	and eax, 1
	jnz fle2

	push r10
	push r9
	push r8
	mov eax, deleteTemplateColumn2	
	push rax
	mov eax, deleteTemplateRow2						
	push rax

	mov edx, 1
	mov ecx, r8d
	shl edx, cl
	add edx, deleteTemplateColumn2
	mov r8d, 1
	mov ecx, r9d
	shl r8d, cl
	add r8d, deleteTemplateRow2	
	mov ecx, pointerMatrix

	call rank

	pop rdx
	mov deleteTemplateRow2, edx
	pop rdx
	mov deleteTemplateColumn2, edx
	pop r8
	pop r9
	pop r10

	cmp eax, rankValue
	jle fle2
	mov rankValue, eax

fle2:
	inc r9d
	cmp r9d, sizeMatrix	
	jne fl2
	xor r9d, r9d

fle1:
	inc r8d															; i++
	cmp r8d, sizeMatrix												; is i=size
	jne fl1
	xor r8d, r8d

omega:
	mov eax, rankValue
	ret

fif1:
	mov eax, sizeMatrix
	sub eax, r10d	
	ret
rank endp

end