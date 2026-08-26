import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";

export function UsbPhysics() {
  return (
    <div className="grid gap-4 lg:grid-cols-2">
      <Card>
        <CardHeader>
          <CardTitle>USB Full Speed · 你现在的硬件</CardTitle>
          <CardDescription>
            RP2040 / RP2350 / nRF52840 / ESP32-S3 片上 USB 都是 12 Mbps Full
            Speed。总线每 1 ms 一个帧（SOF），中断端点每个帧最多被主机问一次。
          </CardDescription>
        </CardHeader>
        <CardContent className="space-y-3">
          <FrameStrip
            label="1 ms 一格 = 1000 Hz"
            cells={8}
            fillEvery={1}
            caption="bInterval = 1 已经是满速。改成 0 非法，改成 2 只会更慢。"
          />
          <p className="text-sm leading-6 text-muted-foreground">
            你仓库{" "}
            <code className="rounded bg-muted px-1.5 py-0.5 font-mono text-xs">
              headers/drivers/xinput/XInputDescriptors.h
            </code>{" "}
            里 IN 端点已经是{" "}
            <code className="rounded bg-muted px-1.5 py-0.5 font-mono text-xs">
              0x01 // bInterval 1
            </code>
            。固件侧没有「1k 开关」可拧到 8k。
          </p>
        </CardContent>
      </Card>
      <Card>
        <CardHeader>
          <CardTitle>USB High Speed · 8 kHz 的前提</CardTitle>
          <CardDescription>
            USB 2.0 High Speed（480 Mbps）把帧切成 125 μs 微帧。bInterval = 1
            才等于 8000 Hz。必须有 HS PHY（片上或 ULPI）。
          </CardDescription>
        </CardHeader>
        <CardContent className="space-y-3">
          <FrameStrip
            label="125 μs 一格 = 8000 Hz"
            cells={8}
            fillEvery={1}
            dense
            caption="Teensy 4.1、STM32H743、CH32V307 这类芯片才能谈 8k。"
          />
          <p className="text-sm leading-6 text-muted-foreground">
            TinyUSB 配置里也对 RP2040 写死了 Full Speed：{" "}
            <code className="rounded bg-muted px-1.5 py-0.5 font-mono text-xs">
              BOARD_DEVICE_RHPORT_SPEED → OPT_MODE_FULL_SPEED
            </code>
            。这不是漏写，是芯片能力。
          </p>
        </CardContent>
      </Card>
    </div>
  );
}

function FrameStrip({
  label,
  cells,
  fillEvery,
  caption,
  dense = false,
}: {
  label: string;
  cells: number;
  fillEvery: number;
  caption: string;
  dense?: boolean;
}) {
  return (
    <div className="space-y-2">
      <div className="flex items-end justify-between text-xs text-muted-foreground">
        <span>{label}</span>
        <span>{dense ? "1 ms 里 8 个微帧" : "8 ms 窗口"}</span>
      </div>
      <div className="grid grid-cols-8 gap-1">
        {Array.from({ length: cells }).map((_, i) => (
          <div
            key={i}
            className={`h-10 rounded-md ${
              i % fillEvery === 0
                ? dense
                  ? "bg-cyan-400/80"
                  : "bg-amber-400/80"
                : "bg-muted"
            }`}
          />
        ))}
      </div>
      <p className="text-xs leading-5 text-muted-foreground">{caption}</p>
    </div>
  );
}
