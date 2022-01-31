		.cpu cortex-m0plus
		.thumb
		.fpu softvfp
		.syntax unified
		.file "clignotant.S"
		.text
		.align 1
		.global main2
		.thumb_func
		.type main2, %function

		.equ GPIO_PORT_NOT0,0xa0002300
		.equ blue,(1<<11)
		.equ init_cpt,200000
		.equ SYSAHBCLKCTRL0,0x40048080
		.equ IOCON,(1<<18)
		.equ GPIO0,(1<<6)
		.equ GPIO_PORT_DIR0,0xa0002000

main2:
		push {r4, r5, r6, lr} @ sauvegarde contexte (inutile)
		ldr r4, =SYSAHBCLKCTRL0
		ldr r5, =IOCON|GPIO0
		str r5, [r4, #0]			@ activation GPIO
		ldr r4, =GPIO_PORT_DIR0
		ldr r5, =blue
		str r5, [r4, #0]			@ bit 11 en sortie
		ldr r4, =GPIO_PORT_NOT0
infini:
		str r5, [r4, #0]			@ complementation état LED
		ldr r6, =init_cpt
attente:
		subs r6, #1
		bne attente
		b infini
		pop {r4, r5, r6, pc} @ jamais execute

		.size main2, .-main2
