import { ArchitectureTabs } from "@/components/report/architecture-tabs";
import { BoardFinder } from "@/components/report/board-finder";
import { Faq } from "@/components/report/faq";
import { FirmwareScan } from "@/components/report/firmware-scan";
import { UsbPhysics } from "@/components/report/usb-physics";
import { VerdictCards } from "@/components/report/verdict-cards";
import { Badge } from "@/components/ui/badge";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { Separator } from "@/components/ui/separator";

const nav = [
  { href: "#verdict", label: "结论" },
  { href: "#firmware", label: "固件" },
  { href: "#usb", label: "1k → 8k" },
  { href: "#trimode", label: "三模" },
  { href: "#boards", label: "开发板" },
  { href: "#faq", label: "误区" },
];

export default function Home() {
  return (
    <div className="flex flex-1 flex-col">
      <header className="sticky top-0 z-30 border-b border-border/80 bg-background/80 backdrop-blur-md">
        <div className="mx-auto flex max-w-6xl flex-col gap-3 px-4 py-3 sm:flex-row sm:items-center sm:justify-between">
          <div className="flex items-center gap-2">
            <span className="size-2 rounded-full bg-cyan-400 shadow-[0_0_12px_var(--color-cyan-400)]" />
            <span className="font-medium tracking-tight">GP2040-CE 三模 / 8k 评估</span>
          </div>
          <nav className="flex flex-wrap gap-x-4 gap-y-1 text-sm text-muted-foreground">
            {nav.map((item) => (
              <a
                key={item.href}
                href={item.href}
                className="hover:text-foreground"
              >
                {item.label}
              </a>
            ))}
          </nav>
        </div>
      </header>

      <main className="mx-auto flex w-full max-w-6xl flex-1 flex-col gap-16 px-4 py-10 sm:py-14">
        <section className="space-y-6">
          <div className="flex flex-wrap gap-2">
            <Badge variant="secondary">sunga2022/GP2040-CE</Badge>
            <Badge variant="outline">RP2040 · USB Full Speed</Badge>
            <Badge variant="outline">现测 1000 Hz</Badge>
          </div>
          <div className="max-w-3xl space-y-4">
            <h1 className="text-balance text-3xl font-semibold tracking-tight sm:text-4xl">
              现有固件做不了蓝牙 2.4G 有线三模 8k。1k 已经顶到 USB 物理上限。
            </h1>
            <p className="text-pretty text-base leading-7 text-muted-foreground sm:text-lg">
              对着你仓库的驱动、USB 描述符和 17 套自定义板看过：这是一套成熟的
              RP2040 有线格斗板固件，XInput 平均延迟 0.76
              ms。要三模就外挂射频 MCU；要 8k 就换 High Speed
              芯片并重写 USB 栈。两条路都不是改几个宏能完成的。
            </p>
          </div>
        </section>

        <section id="verdict" className="scroll-mt-24 space-y-4">
          <SectionHeading
            kicker="三句话"
            title="先看结论，再决定买哪块板"
          />
          <VerdictCards />
        </section>

        <section id="firmware" className="scroll-mt-24 space-y-4">
          <SectionHeading
            kicker="仓库现状"
            title="GP2040-CE 现在是什么"
          />
          <FirmwareScan />
          <Card>
            <CardHeader>
              <CardTitle>主循环在干什么</CardTitle>
              <CardDescription>
                src/gp2040.cpp 的 run() 是一条忙等环：读 GPIO → 插件 →
                协议驱动组包 → tud_task()。没有独立的「轮询率设置」，速率由 USB
                主机按描述符来问。
              </CardDescription>
            </CardHeader>
            <CardContent>
              <pre className="overflow-x-auto rounded-lg bg-muted/70 p-4 font-mono text-[12px] leading-6 text-foreground">
{`while (1) {
  debounceGpioGetAll();
  gamepad->read();
  addons.PreprocessAddons();
  gamepad->process();          // SOCD / 热键
  addons.ProcessAddons();
  inputDriver->process(gamepad); // XInput / PS5 / Switch ...
  tud_task();                    // TinyUSB，等主机下一帧来问
}`}
              </pre>
            </CardContent>
          </Card>
        </section>

        <section id="usb" className="scroll-mt-24 space-y-4">
          <SectionHeading
            kicker="1 kHz → 8 kHz"
            title="卡在 USB 速度，不在固件优化"
          />
          <UsbPhysics />
          <div className="grid gap-4 md:grid-cols-3">
            <Stat
              label="现在（XInput）"
              value="0.76 ms"
              hint="官方同架构固件均值，1 ms 轮询"
            />
            <Stat
              label="理论 8 kHz 均值"
              value="~0.06 ms"
              hint="只对 PC 有意义，主机不吃"
            />
            <Stat
              label="一帧格斗游戏"
              value="16.7 ms"
              hint="60 fps。1k 已经远小于一帧"
            />
          </div>
        </section>

        <section id="trimode" className="scroll-mt-24 space-y-4">
          <SectionHeading
            kicker="蓝牙 + 2.4G + 有线"
            title="三模要拆架构，不要在 RP2040 里硬塞"
          />
          <div className="grid gap-4 md:grid-cols-3">
            <ModeCard
              title="有线"
              now="已经是产品级"
              limit="RP2040 USB FS = 1 kHz"
              extra="PS5 / Xbox 认证走 PIO-USB 从口，这是你板子上最值钱的部分。"
            />
            <ModeCard
              title="2.4G"
              now="仓库里没有"
              limit="要私有协议 + 配套接收器"
              extra="Nordic ESB 做 1 kHz 很现实。8 kHz 需要接收器也是 USB HS，等于第二套产品。"
            />
            <ModeCard
              title="蓝牙"
              now="仓库里没有"
              limit="标准 BLE HID ≈ 133 Hz"
              extra="给手机、平板、偶尔 Switch。不要指望 BLE 打竞技 1k，更不要指望 8k。"
            />
          </div>
          <ArchitectureTabs />
        </section>

        <section id="boards" className="scroll-mt-24 space-y-4">
          <SectionHeading
            kicker="买什么板"
            title="评估板按目标选，不要混着用"
          />
          <BoardFinder />
        </section>

        <section id="faq" className="scroll-mt-24 space-y-4">
          <SectionHeading kicker="对照" title="动手前先避开这些坑" />
          <Faq />
        </section>

        <Separator />

        <section className="grid gap-4 md:grid-cols-3">
          <NextStep
            step="1"
            title="继续现有有线产品"
            body="G2 / RUYI / doio 这套 RP2040 板继续吃 GP2040-CE。1 kHz 对格斗已经足够，主机认证不要拆。"
          />
          <NextStep
            step="2"
            title="若要三模，先做双 MCU 原型"
            body="Pico 保持原样，UART 接到 XIAO nRF52840。再买一块 nRF 当 2.4G 接收器。先跑通 PC 1k，再谈量产 PCB。"
          />
          <NextStep
            step="3"
            title="若只是好奇 8k"
            body="单独买 Teensy 4.1 做 HID 手柄，用轮询检测软件看是不是真 8000。不要把它和现有固件混在一个工程里。"
          />
        </section>
      </main>

      <footer className="border-t border-border py-6 text-center text-xs text-muted-foreground">
        基于 github.com/sunga2022/GP2040-CE 源码与 USB 2.0 规范的技术评估，不是官方 GP2040-CE 文档。
      </footer>
    </div>
  );
}

function SectionHeading({
  kicker,
  title,
}: {
  kicker: string;
  title: string;
}) {
  return (
    <div className="space-y-1">
      <p className="text-xs font-medium tracking-[0.18em] text-cyan-400 uppercase">
        {kicker}
      </p>
      <h2 className="text-2xl font-semibold tracking-tight">{title}</h2>
    </div>
  );
}

function Stat({
  label,
  value,
  hint,
}: {
  label: string;
  value: string;
  hint: string;
}) {
  return (
    <Card>
      <CardHeader>
        <CardDescription>{label}</CardDescription>
        <CardTitle className="font-mono text-3xl">{value}</CardTitle>
        <CardDescription>{hint}</CardDescription>
      </CardHeader>
    </Card>
  );
}

function ModeCard({
  title,
  now,
  limit,
  extra,
}: {
  title: string;
  now: string;
  limit: string;
  extra: string;
}) {
  return (
    <Card>
      <CardHeader>
        <CardTitle>{title}</CardTitle>
        <CardDescription className="space-y-2 leading-6">
          <span className="block">现状：{now}</span>
          <span className="block">上限：{limit}</span>
          <span className="block">{extra}</span>
        </CardDescription>
      </CardHeader>
    </Card>
  );
}

function NextStep({
  step,
  title,
  body,
}: {
  step: string;
  title: string;
  body: string;
}) {
  return (
    <Card>
      <CardHeader>
        <Badge variant="outline">下一步 {step}</Badge>
        <CardTitle>{title}</CardTitle>
        <CardDescription className="leading-6">{body}</CardDescription>
      </CardHeader>
    </Card>
  );
}
