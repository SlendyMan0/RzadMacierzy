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
; Apparently despite the name "general purpose", you shouldn't use RBX, RDI, RSI, RBP, R12-15, for some yet unknown for me reason
;
; PASSED ARGUMENTS:
;
; 1 parameter: RCX
; 2 parameter: RDX
; 3 parameter: R8
; 4 parameter: R9
; rest in stack
;
; DATA SEGMENT
;
; because I'm an idiot and forgot about it.
; .data to start data segment, before code segment
; [label] [size] [initialValue/?] - if you dont care about value, use ?
;
; sizes:
;
; BYTE: db, byte, sbyte(for signed byte)
; WORD: dw, word, sword(for signed word)
; DWORD: dd, dword, sdword(for signed dword)
; QWORD: qd, qword, sqword(for signed qword)
;
; arrays:
;
; [label] [size] [numberOfElements] dup ([initialValueOfEachElement]) - 1d array
; [label] [size] [numberOfRows] dup ([numberOfColumns] dup [initialValueOfEachElement]) - 2d array
.data

pointerMatrix dword ?
deleteTemplateRow dword ?
deleteTemplateColumn dword ?
sizeMatrix dword ?

.code
determinant proc
	mov pointerMatrix, ecx											; saving pointer to matrix to memory
	mov deleteTemplateColumn, edx									; saving deleteTemplateColumn to memory
	mov deleteTemplateRow, r8d										; saving deleteTemplateRow to memory
	mov eax, dword ptr [ecx]
	mov sizeMatrix, eax												; saving size of matrix to memory
	xor r10d, r10d													; clearing R10D for use
	xor ecx, ecx													; loop counter to zero
l1:	
	mov eax, deleteTemplateColumn									; copying deleteTemplateColumn
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
l2: 
	mov eax, deleteTemplateColumn									; copying deleteTemplateColumn
	shr eax, cl														; shifing deleteTemplateColumn to compare bits
	and eax, 1														; comparing bits
	jz jcf															; jumping to set the number correctly (jump column found)
jbc:
	mov eax, deleteTemplateRow										; copying deleteTemplateRow
	shr eax, cl														; shifing deleteTemplateColumn to compare bits
	and eax, 1														; comparing bits
	jz jrf															; jumping to set the number correctly (jump row found)
jbr:
	inc ecx															; i++
	cmp ecx, sizeMatrix												; is i=size
	jne l2	

	mov ecx, sizeMatrix												; preparing to calculate the exact position of number to return 
	imul ecx, r10d													; multiplying by row index
	add ecx, r9d													; adding column index
	inc ecx															; remembering that first position in matrix is it's size
	shl ecx, 2														; remembering that dword is 4 bytes
	add ecx, pointerMatrix											; calculating pointer

	mov eax, dword ptr [ecx]										; returning value
	ret
jcf:
	mov r9d, ecx													; saving column index
	jmp jbc															; jumping back (jump back column)
jrf:
	mov r10d, ecx													; saving row index
	jmp jbr															; jumping back (jump back row)
j1:																	
	xor ecx, ecx													; loop counter to zero
l3:																	; finding first non-excluded row
	mov eax, deleteTemplateRow										; copying deleteTemplateRow
	shr eax, cl														; shifing deleteTemplateRow to compare bits
	inc ecx															; i++
	and eax, 1														; comparing bits
	jnz l3															; jump if excluded

	dec ecx															; i++ was not needed, non-excluded row found
	mov r9d, ecx													; saving firstEmptyRow
	xor ecx, ecx													; loop counter to zero
	xor r10d, r10d													; clearing k counter
l4:
	mov eax, deleteTemplateColumn									; copying deleteTemplateColumn
	shr eax, cl														; shifing deleteTemplateColumn to compare bits
	and eax, 1														; comparing bits
	jnz skip
	inc r10d														; k++

	mov eax, 1														; pow(-1, (1 + k + firstEmptyRow))
	add eax, r9d													; + firstEmptyRow
	add eax, r10d													; + k
	and eax, 1														; checking the paritiy
	push rax														; saving pairity check for later													
	mov eax, sizeMatrix												; matrix[(size*firstEmptyRow) + i + 1]
	imul eax, r9d													; (size*firstEmptyRow)
	add eax, ecx													; + i
	inc eax															; + 1
	shl eax, 2														; remembering that dword is 4 bytes
	add eax, pointerMatrix											; calculating pointer
	
	imul r10d, dword ptr [eax]										; pow(-1, (1 + k + firstEmptyRow)) * matrix[(size*firstEmptyRow) + i + 1]

	mov eax, deleteTemplateColumn									; saving data on stack
	push rax
	mov eax, deleteTemplateRow										
	push rax
	push r10
	push r9
	push rcx

	mov rdx, 1														; setting second parameter
	shl edx, cl														; pow(2, i)
	add edx, deleteTemplateColumn									; + deleteTemplateColumn
	mov r8d, 1														; setting third parameter
	mov ecx, r9d
	shl r8d, cl														; pow(2, firstEmptyRow)
	add r8d, deleteTemplateRow										; + deleteTemplateRow
	mov ecx, pointerMatrix											; setting first parameter

	call determinant												; determinant(matrix, deleteTemplateColumn + pow(2, i), deleteTemplateRow + pow(2, firstEmptyRow))

	pop rcx															; resetting values
	pop r9
	pop r10
	pop rax
	mov deleteTemplateRow, eax
	pop rax
	mov deleteTemplateColumn, eax	

	imul r10d, eax													; final multiplying
	pop rax
	cmp al, 0
	je aDet
	sub r11d, r10d
	jmp skip
aDet:
	add r11d, r10d													; det+=
skip:
	inc ecx															; i++
	cmp ecx, sizeMatrix												; i=size?
	jne l4

	mov eax, r11d
	ret
determinant endp

rank proc
	xor r11d, r11d

	call determinant

	ret
rank endp

end