# GP2040-CE 三模 / 8k 可行性评估

对着 [sunga2022/GP2040-CE](https://github.com/sunga2022/GP2040-CE) 源码做的技术结论页，回答三件事：

1. **现有固件能不能改成蓝牙 + 2.4G + 有线三模？** 不能在 RP2040 上直接加。仓库没有射频栈，芯片也没有射频。务实做法是外挂 nRF52840，有线继续跑 GP2040-CE。
2. **1 kHz 能不能改成 8 kHz？** 不能。USB 描述符里 `bInterval` 已经是 1；RP2040 只有 USB Full Speed，1 ms 一帧就是 1000 Hz 上限。8 kHz 需要 USB High Speed MCU，并重写 USB 栈。
3. **该买哪些开发板？** 按目标选：继续有线用现有 RP2040 板；三模 1k 用 XIAO nRF52840；有线 8k 用 Teensy 4.1 / STM32H743 / CH32V307。

这不是 GP2040-CE 固件本身，也不会去改你那个仓库。

## 本地运行

需要 Node.js 18+。

```bash
npm install
npm run dev
```

浏览器打开 [http://127.0.0.1:43211](http://127.0.0.1:43211)。

```bash
npm run build
npm start
```

## 和固件仓库的关系

固件仍在 `https://github.com/sunga2022/GP2040-CE`。本仓库只放评估页，方便对照 USB 物理限制、架构方案和开发板。
