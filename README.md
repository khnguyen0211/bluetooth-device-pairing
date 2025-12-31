# Bluetooth Device Pairing Tool

Ứng dụng command-line để discover và pair thiết bị Bluetooth trên Windows, được xây dựng bằng C++ và Qt Framework.

## Mục lục

- [Architecture](#architecture)
- [Cấu trúc Project](#cấu-trúc-project)
- [Các API chính](#các-api-chính)
- [Workflows](#workflows)
- [Bypass Security Confirmation](#bypass-security-confirmation)
- [Cách sử dụng](#cách-sử-dụng)

---

## Architecture

Project được thiết kế theo các nguyên tắc OOP, SOLID và sử dụng các Design Patterns phổ biến:

```
┌─────────────────────────────────────────────────────────────┐
│                     Application Layer                       │
│                        main.cpp                             │
│         (Configuration, Entry Point, Flow Control)          │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   Business Logic Layer                      │
│  ┌─────────────────┐              ┌─────────────────┐       │
│  │ DeviceDiscoverer│              │  DevicePairer   │       │
│  └─────────────────┘              └─────────────────┘       │
│              │                            │                 │
│              └────────────┬───────────────┘                 │
│                           ▼                                 │
│                  ┌─────────────────┐                        │
│                  │ BluetoothManager│ (Singleton)            │
│                  └─────────────────┘                        │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│              Platform Abstraction Layer                     │
│                   (Strategy Pattern)                        │
│                                                             │
│            ┌─────────────────────┐                          │
│            │  IPlatformStrategy  │ (Interface)              │
│            └─────────────────────┘                          │
│                       ▼                                     │
│                       │                                     │
│            ┌─────────────────────┐                          │
│            │   WindowsStrategy   │ (Concrete Implementation)│
│            └─────────────────────┘                          │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Qt Bluetooth Layer                       │
│  ┌────────────────────────────────────────────────────────┐ │
│  │  QBluetoothDeviceDiscoveryAgent                        │ │
│  │  QBluetoothLocalDevice                                 │ │
│  │  QBluetoothDeviceInfo                                  │ │
│  │  QBluetoothAddress                                     │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Design Patterns sử dụng

| Pattern | Áp dụng | Mục đích |
|---------|---------|----------|
| **Strategy** | `IPlatformStrategy` → `WindowsStrategy` | Cho phép mở rộng sang Linux/macOS trong tương lai |
| **Singleton** | `BluetoothManager` | Quản lý một instance duy nhất cho Bluetooth operations |
| **Value Object** | `MacAddress`, `DiscoveryTime` | Đảm bảo tính immutable và validation |

---

## Cấu trúc Project

```
src/
├── main.cpp                          # Entry point, configuration
│
├── bluetooth/                        # Business Logic Layer
│   ├── BluetoothManager.h/cpp        # Singleton quản lý Bluetooth
│   ├── DeviceDiscoverer.h/cpp        # Logic discover devices
│   └── DevicePairer.h/cpp            # Logic pair devices
│
├── platform/                         # Platform Abstraction Layer
│   ├── IPlatformStrategy.h           # Interface cho platform operations
│   └── WindowsStrategy.h/cpp         # Windows implementation
│
├── models/                           # Data Models
│   ├── MacAddress.h/cpp              # Value object cho MAC address
│   ├── DiscoveryTime.h/cpp           # Value object cho discovery time
│   ├── BluetoothDevice.h/cpp         # Model cho Bluetooth device
│   └── DeviceFormatter.h/cpp         # Format output cho console
│
└── utils/                            # Utilities
    └── AppException.h/cpp            # Custom exception class
```

---

## Các API chính

### Qt Bluetooth APIs

#### 1. QBluetoothDeviceDiscoveryAgent
Dùng để scan và discover các thiết bị Bluetooth xung quanh.

```cpp
// Khởi tạo
m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

// Cấu hình timeout cho BLE devices
m_discoveryAgent->setLowEnergyDiscoveryTimeout(10000);

// Signals quan trọng
connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
        this, &WindowsStrategy::onDeviceDiscovered);
connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
        this, &WindowsStrategy::onDiscoveryFinished);
connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred,
        this, &WindowsStrategy::onDiscoveryError);

// Bắt đầu scan
m_discoveryAgent->start();

// Dừng scan
m_discoveryAgent->stop();
```

#### 2. QBluetoothLocalDevice
Đại diện cho Bluetooth adapter trên máy local, dùng để pair/unpair devices.

```cpp
// Khởi tạo
m_localDevice = new QBluetoothLocalDevice(this);

// Kiểm tra adapter có valid không
if (!m_localDevice->isValid()) {
    throw AppException("No Bluetooth adapter found");
}

// Bật Bluetooth nếu đang tắt
if (m_localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
    m_localDevice->powerOn();
}

// Request pairing
m_localDevice->requestPairing(address, QBluetoothLocalDevice::Paired);

// Signals quan trọng
connect(m_localDevice, &QBluetoothLocalDevice::pairingFinished,
        this, &WindowsStrategy::onPairingFinished);
connect(m_localDevice, &QBluetoothLocalDevice::errorOccurred,
        this, &WindowsStrategy::onPairingError);
```

#### 3. QBluetoothDeviceInfo
Chứa thông tin về một thiết bị Bluetooth được discover.

```cpp
// Lấy thông tin device
QString name = info.name();
QBluetoothAddress address = info.address();

// Kiểm tra loại device (Classic vs BLE)
bool isBLE = info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration;
```

#### 4. QBluetoothAddress
Đại diện cho MAC address của Bluetooth device.

```cpp
// Tạo từ string
QBluetoothAddress address("AA:BB:CC:DD:EE:FF");

// Tạo từ số
QBluetoothAddress address(0xAABBCCDDEEFF);

// Convert sang string
QString macString = address.toString();
```

---

## Workflows

### Discovery Workflow

```
┌─────────────────────────────────────────────────────────────┐
│                    USER: Run Application                    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│              BluetoothManager::initialize()                 │
│  - Tạo WindowsStrategy instance                             │
│  - Setup QBluetoothDeviceDiscoveryAgent                     │
│  - Setup QBluetoothLocalDevice                              │
│  - Kiểm tra Bluetooth adapter valid                         │
│  - Bật Bluetooth nếu đang tắt                               │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│           DeviceDiscoverer::discoverDevices()               │
│  - Tạo DiscoveryTime object (validate 1-30 seconds)         │
│  - Gọi WindowsStrategy::discoverDevices()                   │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│           WindowsStrategy::discoverDevices()                │
│  1. Clear danh sách devices cũ                              │
│  2. Set m_discoveryFinished = false                         │
│  3. Tạo QTimer với timeout = discoveryTime                  │
│  4. Gọi m_discoveryAgent->start()                           │
│  5. Vòng lặp processEvents() cho đến khi finished           │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│              [ASYNC] Device Discovery Process               │
│                                                             │
│  Mỗi khi tìm thấy device:                                   │
│  ┌────────────────────────────────────────────────────────┐ │
│  │ Signal: deviceDiscovered(QBluetoothDeviceInfo)         │ │
│  │    │                                                   │ │
│  │    ▼                                                   │ │
│  │ onDeviceDiscovered():                                  │ │
│  │  - Kiểm tra address không null                         │ │
│  │  - Convert QBluetoothDeviceInfo → BluetoothDevice      │ │
│  │  - Thêm vào m_discoveredDevices list                   │ │
│  └────────────────────────────────────────────────────────┘ │
│                                                             │
│  Khi timeout hoặc scan xong:                                │
│  ┌────────────────────────────────────────────────────────┐ │
│  │ Signal: finished() hoặc Timer::timeout()               │ │
│  │    │                                                   │ │
│  │    ▼                                                   │ │
│  │ Set m_discoveryFinished = true                         │ │
│  │ → Thoát vòng lặp processEvents()                       │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│              Return QList<BluetoothDevice>                  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│           DeviceFormatter::printDeviceTable()               │
│  - Sort devices theo name                                   │
│  - Format và in ra console dạng table                       │
│  - Hiển thị: Type | MAC Address | Status | Name             │
└─────────────────────────────────────────────────────────────┘
```

### Pairing Workflow

```
┌─────────────────────────────────────────────────────────────┐
│                    USER: Run Pairing                        │
│              (với TARGET_MAC_ADDRESS đã config)             │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                DevicePairer::pairDevice()                   │
│  - Validate MAC address format                              │
│  - Gọi WindowsStrategy::findDeviceByMac() để tìm device     │
│  - Kiểm tra device có tồn tại không                         │
│  - Kiểm tra connection status                               │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│            WindowsStrategy::pairDevice()                    │
│  1. Set m_pairingSuccess = false                            │
│  2. Set m_pairingFinished = false                           │
│  3. Convert MAC string → QBluetoothAddress                  │
│  4. Gọi m_localDevice->requestPairing(address, Paired)      │
│  5. Tạo QTimer với timeout = 30 seconds                     │
│  6. Vòng lặp processEvents() cho đến khi finished           │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│              [ASYNC] Pairing Process                        │
│                                                             │
│  Qt Bluetooth tự động chọn pairing method phù hợp:          │
│  ┌────────────────────────────────────────────────────────┐ │
│  │ • Just Works: Auto-accept (tai nghe, loa, chuột...)    │ │
│  │ • Numeric Comparison: So sánh số 6 chữ số              │ │
│  │ • Passkey Entry: Nhập PIN (keyboard)                   │ │
│  └────────────────────────────────────────────────────────┘ │
│                                                             │
│  Khi pairing hoàn tất:                                      │
│  ┌────────────────────────────────────────────────────────┐ │
│  │ Signal: pairingFinished(address, pairing_status)       │ │
│  │    │                                                   │ │
│  │    ▼                                                   │ │
│  │ onPairingFinished():                                   │ │
│  │  - Kiểm tra status: Paired / AuthorizedPaired → SUCCESS│ │
│  │  - Unpaired / Cancelled → FAILED                       │ │
│  │  - Set m_pairingFinished = true                        │ │
│  └────────────────────────────────────────────────────────┘ │
│                                                             │
│  Nếu có lỗi:                                                │
│  ┌────────────────────────────────────────────────────────┐ │
│  │ Signal: errorOccurred(error)                           │ │
│  │    │                                                   │ │
│  │    ▼                                                   │ │
│  │ onPairingError():                                      │ │
│  │  - Log error code                                      │ │
│  │  - Set m_pairingSuccess = false                        │ │
│  │  - Set m_pairingFinished = true                        │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   Return bool (success)                     │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Display Result                           │
│  - SUCCESS: "Device paired!"                                │
│  - FAILED: "Pairing failed!"                                │
└─────────────────────────────────────────────────────────────┘
```

---

## Bypass Security Confirmation

### Vấn đề phát hiện

Khi pair Bluetooth Keyboard qua **Windows Settings UI**, hệ thống luôn yêu cầu nhập mã PIN để xác nhận. Tuy nhiên, khi sử dụng **Qt Bluetooth API trực tiếp**, keyboard có thể được pair thành công mà **không cần nhập PIN**.

### Nguyên nhân kỹ thuật

#### 1. Bluetooth Secure Simple Pairing (SSP)

Theo [Bluetooth SIG Specification](https://www.bluetooth.com/specifications/specs/), từ Bluetooth 2.1 trở đi, **Secure Simple Pairing (SSP)** được giới thiệu để đơn giản hóa quá trình pairing. SSP định nghĩa 4 phương thức pairing dựa trên **IO Capabilities** của thiết bị:

| Method | Mô tả | MITM Protection |
|--------|-------|-----------------|
| **Just Works** | Auto-accept, không cần xác nhận | ❌ Không |
| **Numeric Comparison** | Hiển thị số 6 chữ số để so sánh | ✅ Có |
| **Passkey Entry** | Một bên hiển thị PIN, bên kia nhập | ✅ Có |
| **Out of Band (OOB)** | Dùng NFC hoặc phương thức khác | Tùy thuộc OOB |

> **Nguồn**: [Silicon Labs - Bluetooth Pairing Mechanisms](https://docs.silabs.com/bluetooth/latest/bluetooth-security-pairing-processes/)

#### 2. IO Capabilities và Pairing Method Selection

Bluetooth stack tự động chọn pairing method dựa trên **IO Capabilities** của cả 2 thiết bị. Bảng mapping theo Bluetooth Core Specification:

| Initiator \ Responder | DisplayOnly | DisplayYesNo | KeyboardOnly | NoInputNoOutput | KeyboardDisplay |
|-----------------------|-------------|--------------|--------------|-----------------|-----------------|
| **DisplayOnly** | Just Works | Just Works | Passkey Entry | Just Works | Passkey Entry |
| **DisplayYesNo** | Just Works | Numeric Comparison | Passkey Entry | Just Works | Numeric Comparison |
| **KeyboardOnly** | Passkey Entry | Passkey Entry | Passkey Entry | Just Works | Passkey Entry |
| **NoInputNoOutput** | Just Works | Just Works | Just Works | Just Works | Just Works |
| **KeyboardDisplay** | Passkey Entry | Numeric Comparison | Passkey Entry | Just Works | Numeric Comparison |

> **Nguồn**: [Silicon Labs - Security Pairing Processes](https://docs.silabs.com/bluetooth/latest/bluetooth-security-pairing-processes/)

#### 3. Tại sao Qt API có thể bypass PIN?

Theo [Qt Documentation - QBluetoothLocalDevice](https://doc.qt.io/qt/qbluetoothlocaldevice.html):

> *"On Windows the exact pairing mode decision is up to the operating system."*

Khi gọi `requestPairing()`:

1. **Qt không khai báo IO Capabilities cụ thể** → Bluetooth stack có thể chọn `NoInputNoOutput`
2. **Với NoInputNoOutput** → Theo bảng trên, **mọi trường hợp đều fallback về Just Works**
3. **Just Works** = Pair ngay lập tức, không cần user interaction

```cpp
// Qt API - không có cơ chế để specify IO capabilities
m_localDevice->requestPairing(address, QBluetoothLocalDevice::Paired);

// Kết quả: Bluetooth stack tự chọn Just Works nếu có thể
```

#### 4. Windows Settings UI vs Direct API

| Phương thức | IO Capabilities | Pairing Method |
|-------------|-----------------|----------------|
| **Windows Settings UI** | DisplayYesNo hoặc KeyboardDisplay | Passkey Entry / Numeric Comparison |
| **Qt Bluetooth API** | NoInputNoOutput (default) | Just Works |

Windows Settings UI **cố tình** khai báo IO capabilities cao hơn để enforce security policy, trong khi Qt API để OS tự quyết định.

### Thiết bị hỗ trợ nhiều methods

Nhiều keyboard hiện đại (đặc biệt là Bluetooth 4.0+) hỗ trợ **cả Passkey Entry và Just Works**:

- **Khi pair qua Windows UI**: Windows khai báo có keyboard → Passkey Entry được chọn
- **Khi pair qua Qt API**: Qt không khai báo IO → Just Works được chọn

> **Lưu ý**: Không phải tất cả keyboard đều hỗ trợ Just Works. Một số keyboard cũ chỉ hỗ trợ Legacy Pairing với fixed PIN.

### Security Implications

| Aspect | Đánh giá |
|--------|----------|
| **Ưu điểm** | Tiện lợi cho automation, scripting, batch pairing |
| **Nhược điểm** | Không có MITM protection, có thể pair thiết bị mà user không biết |
| **Risk Level** | Medium - Cần physical proximity để scan được device |

> **Cảnh báo từ Bluetooth SIG**: *"Just Works provides no man-in-the-middle (MITM) protection"* - [Wikipedia - Bluetooth Pairing](https://en.wikipedia.org/wiki/Bluetooth#Pairing_and_bonding)

### Khuyến nghị

1. **Cho mục đích automation/development**: Giữ nguyên behavior hiện tại
2. **Cho production application**: Cân nhắc implement `pairingDisplayPinCode` và `pairingDisplayConfirmation` signals
3. **Cho security-sensitive environment**: Sử dụng Windows native API với explicit IO capabilities

### Tham khảo thêm

- [Qt Documentation - QBluetoothLocalDevice](https://doc.qt.io/qt/qbluetoothlocaldevice.html)
- [Silicon Labs - Bluetooth Security Pairing Processes](https://docs.silabs.com/bluetooth/latest/bluetooth-security-pairing-processes/)
- [Bluetooth SIG - Core Specification](https://www.bluetooth.com/specifications/specs/)
- [Stack Overflow - How to force a bluetooth pairing pin with QT?](https://stackoverflow.com/questions/39289001/how-to-force-a-bluetooth-pairing-pin-with-qt)

---

## Cách sử dụng

### Configuration

Mở file `src/main.cpp` và chỉnh sửa các giá trị:

```cpp
// Discovery settings
const int DISCOVERY_TIME_SECONDS = 10;  // Thời gian scan (1-30 giây)

// Pairing settings
const QString TARGET_MAC_ADDRESS = "AA:BB:CC:DD:EE:FF";  // MAC của device cần pair
const QString PIN_CODE = "0000";  // PIN code (nếu cần)

// Set true để chạy pairing sau khi discover
const bool RUN_PAIR_AFTER_DISCOVER = true;
```

### Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64"
cmake --build . --config Debug
```

### Run

```bash
./Debug/bluetooth-device-pairing.exe
```

### Output mẫu

```
Discovering devices for 10 seconds...

Found 5 device(s):
-------------------------------------------------------------------------
|  |AA:BB:CC:DD:EE:FF|         |My Keyboard                             |
|LE|11:22:33:44:55:66|Paired   |SOUNDPEATS T3 Pro                       |
|  |77:88:99:AA:BB:CC|         |Bluetooth Mouse                         |
-------------------------------------------------------------------------

--- Starting Pairing ---
Starting pairing with device: AA:BB:CC:DD:EE:FF
PIN Code: 0000
Waiting for pairing response...
Pairing finished for: AA:BB:CC:DD:EE:FF
Status: SUCCESS - Device paired!

--- Pairing Result ---
SUCCESS: Device paired!
```

---

## Dependencies

- Qt 6.x (Core, Bluetooth modules)
- CMake 3.16+
- MSVC 2022 (Windows)

## License

MIT License
