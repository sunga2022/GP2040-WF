# 杰里 AC632N：必须单独烧固件

AC632N **一定要烧蓝牙固件**。手柄上的 `GP2040-WF.uf2` 只进 RP2040，**烧不到杰里里**。

## 现成文件（已在 GitHub 上编出来）

仓库 [`firmware-bin/`](../../firmware-bin/)：

| 文件 | 用法 |
|------|------|
| `GP2040-WF_AC632N.ufw` | 杰理烧录助手 / 强制升级，选 AC632N |
| `GP2040-WF_AC632N_app.bin` | 下载器里当 APP |
| `GP2040-WF_AC632N_jl_isd.bin` | 整片镜像，offset 0 |

推 `main` 后，Actions 工作流 **Jieli AC632N HID** 会再编一份，Artifacts 里也能下。

蓝牙名 **Pro Controller**。Pico **GP24 TX**（UART1）→ 模块 UART **RX**（固件默认芯片脚 **PA5**），1 Mbps 8N1，共地 3.3 V。

## 自己在 Linux / GitHub 上编

官方 SDK 支持 `make ac632n_hid`，工具链要杰理的 clang（不是 GitHub 自带的 gcc）：

```bash
# 工具链 → /opt/jieli/common/bin/clang
curl -L https://pkgman.jieliapp.com/s/linux-toolchain | sudo tar -xJ -C /tmp
# 解压后的目录拷到 /opt/jieli
# 后处理工具：
curl -L https://pkgman.jieliapp.com/s/linux-postbuild | sudo tar -xJ -C /tmp

git clone --depth 1 https://github.com/Jieli-Tech/fw-AC63_BT_SDK.git
./firmware/jieli_ac632n/patch_sdk.sh ./fw-AC63_BT_SDK
ulimit -n 8192
make -C fw-AC63_BT_SDK ac632n_hid -j$(nproc) || true   # 自带 download.sh 在 Linux 上会失败，ELF 已经出来
./firmware/jieli_ac632n/pack_bin.sh fw-AC63_BT_SDK/cpu/bd19/tools
```

产出：`app.bin`、`jl_isd.bin`、`jl_isd.ufw`。

Xbox / PS **不要**走这颗蓝牙。只用有线或 2.4G 可以不焊、不烧它。
