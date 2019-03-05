# Mach-O-Hook
Hook The Import Table Of Mach-O To Implement The Function Redirection.
Compared With Previous Implementations (For Example \[Shoumikhin 2014\] https://github.com/shoumikhin/Mach-O-Hook), We Have Two Major Advantages:
1. We Read Data Directly From The Mach-O Image Mapped In The Virual Address Space And Not Need To Open The Mach-O File On The Disk At All.
2. We Support Shared Cache Which Is Not Supported By \[Shoumikhin 2014\]'s Implementation. \[Shoumikhin 2014\]'s Implementation Will Crash If You Want To Redirect The "malloc" and "free" In The "libobjc(.A).dylib" Because The "dyld" Use Shared Cache For "libobjc(.A).dylib".
