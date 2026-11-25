

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

  .global g_pfnVectors
  .global Reset_Handler

  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler

  .rept 29
  .word Default_Handler
  .endr

  .word TIM3_IRQHandler

  .rept 52
  .word Default_Handler
  .endr

  .size g_pfnVectors, . - g_pfnVectors

  .section .text.Reset_Handler,"ax",%progbits
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr   sp, =_estack

  ldr   r0, =_sdata
  ldr   r1, =_edata
  ldr   r2, =_sidata
CopyData:
  cmp   r0, r1
  bge   CopyDataDone
  ldr   r3, [r2], #4
  str   r3, [r0], #4
  b     CopyData
CopyDataDone:

  ldr   r0, =_sbss
  ldr   r1, =_ebss
  movs  r3, #0
ZeroBss:
  cmp   r0, r1
  bge   ZeroBssDone
  str   r3, [r0], #4
  b     ZeroBss
ZeroBssDone:

  ldr   r0, =0xE000ED88
  ldr   r1, [r0]
  orr   r1, r1, #(0xF << 20)
  str   r1, [r0]
  dsb
  isb

  bl    main
  b     .
  .size Reset_Handler, . - Reset_Handler

  .section .text.Default_Handler,"ax",%progbits
  .type Default_Handler, %function
Default_Handler:
Infinite_Loop:
  b     Infinite_Loop
  .size Default_Handler, . - Default_Handler

  .weak NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler

  .weak BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler

  .weak UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler

  .weak SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler

  .weak PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler

  .weak TIM3_IRQHandler
  .thumb_set TIM3_IRQHandler,Default_Handler
