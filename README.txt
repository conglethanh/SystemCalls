I. CÀI ĐẶT SYSCALL PNAMETOID & PIDTONAME
	B1: Tải Linux source kernel (Recommend kernel 4.4.1) và giải nén vào thư mục /usr/src/
	B2: Chuyển vào thư mục: cd /usr/src/linux-4.4.1/
	B3: Copy thư mục source code pnametoid/ vào /usr/src/linux-4.4.1
	B4: Tại thư mục linux-4.4.1, chỉnh sửa Makefile: sudo gedit Makefile
		Ctrl + F để tìm "kernel/" sau đó add thêm pnametoid/ vào dòng đó nếu cài pnametoid, thêm pidtoname nếu cài pidtoname
	B5: Chỉnh sửa file syscalls.h: sudo gedit include/linux/syscalls.h, và add thêm vào dòng cuối
		asmlinkage int sys_pnametoid(char* name);
	B6: Chỉnh sửa bảng syscall: sudo gedit arch/x86/entry/syscalls/syscall_64.tbl
		Thêm vào dòng sau (350 common pnametoid  sys_pnametoid) nếu là pnametoid hoặc (351 common pidtoname  sys_pidtoname) nếu là pidtoname
		có thể chọn số khác thay vì 350, 351 miễn không bị trùng
	B7: Tại linux-4.4.1/ gõ sudo make menuconfig để lưu cấu hình hiện tại, chọn save và exit
	B8: Gõ tiếp  make && make modules_install install && reboot để tiến hành build và cài đặt khá lâu
II. TEST SYSCALL
	B1: Chỉnh sửa file testPnametoid.c và testPidtoname.c ở chỗ gọi Syscall, số truyền vào syscall là mã số mà ta đã add thêm
		khi cài đặt syscall (VD: 350 là pnametoid và 351 là pidtoname)
	B2: Nếu test pnametoid ta compile: gcc testPnametoid.c
		Nếu test pidtoname: gcc testPidtoname.c
	B3: ./a.out để chạy file test
III. HOOK OPEN SYSCALL & WRITE SYSCALL & TEST
	B1: Tìm địa chỉ bảng syscall table cat /boot/System.map-4.4.1 | grep sys_call_table (Tùy phiên bản uname -r)
	B2: Thay thay đổi file open hookOpenSyscall.c và hookWriteSyscall.c ở phần sys_call_table_addr
	B3: cd vào thư mục chưa file code
	B4: gõ make để build
	B5: sudo insmod hookOpenSyscall.ko hoặc sudo insmod hookWriteSyscall.ko tùy trường hợp bạn muốn cài đặt hook nào
	B6: Gõ dmesg -wH để xem file thành quả
	B7: Remove module: sudo rmmod hookOpenSyscall hoặc sudo rmmod hookWriteSyscall
	B8: make clean để xóa hết file đã build lúc nãy.