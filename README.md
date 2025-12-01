## KPL Parser - Week 3

### Build chương trình

Từ thư mục gốc của project :

```bash
cd incompleted
make
```

Sau khi build xong, sẽ tạo ra file thực thi:

- `incompleted/parser`

### Chạy parser trên một file KPL

Ví dụ chạy với `example4.kpl` và lưu log ra file:

```bash
./incompleted/parser test/example4.kpl > out4.txt
```

Hoặc in trực tiếp ra màn hình:

```bash
./incompleted/parser test/example4.kpl
```

### Chạy toàn bộ test ví dụ

Từ thư mục gốc project:

```bash
./incompleted/parser test/example1.kpl > out1.txt
./incompleted/parser test/example2.kpl > out2.txt
./incompleted/parser test/example3.kpl > out3.txt
./incompleted/parser test/example4.kpl > out4.txt
./incompleted/parser test/example5.kpl > out5.txt
```

### So sánh với kết quả mong đợi

Trong thư mục `test/` đã có sẵn các file chuẩn `result*.txt`.  
Có thể dùng `diff` để kiểm tra:

```bash
diff -u test/result1.txt out1.txt
diff -u test/result2.txt out2.txt
diff -u test/result3.txt out3.txt
diff -u test/result4.txt out4.txt
diff -u test/result5.txt out5.txt
```

