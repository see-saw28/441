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

main2:
		push {r4, r5, r6, lr}
		ldr r4, =GPIO_PORT_NOT0
		ldr r5, =blue
		str r5, [r4, #0]
		ldr r6, =init_cpt
boucle:
		subs r6, #1
		bne boucle
		pop {r4, r5, r6, pc}

		.size main2, .-main2
