# 嵌入式系统大作业 — 多传感器采集与定时事件调度

基于正点原子探索者 STM32F407ZGT6 开发板的综合设计，完成电压采集、
磁场方位角解算、阈值报警、液晶实时显示、Flash 历史存储、按键交互与串口上报。

## 硬件平台

| 模块 | 器件 | 接口 |
| --- | --- | --- |
| 主控 | STM32F407ZGT6（168 MHz / 1 MB Flash / 192 KB SRAM） | — |
| 显示 | NT35510 4.3 英寸 480×800 | FSMC Bank1 NE4 |
| 磁力计 | ST480MC（地址 0x0C） | I2C：PB8 / PB9 |
| 电压采集 | 可调电位器 RV1 | ADC1_IN5（PA5） |
| 按键 | KEY0 | PE4 |
| 指示灯 | LED0 红 / LED1 绿 | PF9 / PF10 |
| 风扇指示 | 外部风扇 | PB5 |
| 蜂鸣器 | 板载有源蜂鸣器 | PF8（高电平驱动） |
| 存储 | 片内 Flash 扇区 3 | 0x0800C000 |

## 功能

- 电压大于 1.5 V 时风扇指示置位，蜂鸣器以 1 Hz 间歇鸣叫（节拍由 TIM3 中断产生）
- 方位角大于 50° 时 LED1 以 10 Hz 闪烁
- 液晶实时显示电压、角度、风扇状态与蜂鸣器状态
- 每 1 s 保存一组数据，最近 10 组存入片内 Flash，掉电不丢失
- 短按 KEY0 切换实时 / 历史界面，长按 KEY0 清除历史
- 串口每 1 s 上报一帧数据

## 目录结构

```
main.c                 初始化与启动
app/app.c              采样调度、阈值判定、按键处理、串口上报
app/ui.c               界面绘制
bsp/sys.c              时钟、延时、串口
bsp/lcd.c              FSMC + NT35510 + 5x7 字库
bsp/adc.c              PA5 电压采集
bsp/mag.c              ST480MC 磁力计与角度解算
bsp/led.c              LED 与风扇指示
bsp/key.c              按键短按 / 长按识别
bsp/beep.c             TIM3 1 Hz 中断
bsp/storage.c          片内 Flash 追加存储
tools/gen_compdb.sh    生成 compile_commands.json
```

## 编译与烧写

需要 arm-none-eabi-gcc、make 与 OpenOCD：

```bash
make            # 编译
make flash      # 通过 ST-LINK 烧写
make clean      # 清理
```

也可以使用仓库内的 Dockerfile 构建一致的编译环境：

```bash
docker build -t stm32f407-dev .
docker run --rm -v "$PWD":/work -w /work stm32f407-dev make
```

## 开发环境

Ubuntu 24.04 + arm-none-eabi-gcc 13.2.1 + OpenOCD 0.12.0 + ST-LINK（SWD）
