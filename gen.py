import sys

shell = open(r'C:\Users\Gower\Downloads\BojanOS\src\shell.c', 'w')

shell.write(r'''#include "shell.h"
#include "vga.h"
#include "keyboard.h"
#include "string.h"
#include "io.h"
#include "vfs.h"
#include "timer.h"

static char redirect_file[64];
static int redirect_mode;
static int running_bg;

static void vga_outs(const char* s) {
    if (redirect_mode == 1) {
        fs_append(fs_current, redirect_file, s);
    } else if (redirect_mode == 2) {
        fs_append(fs_current, redirect_file, s);
    } else {
        vga_print(s);
    }
}

static void vga_outc(char c) {
    if (redirect_mode) {
        char t[2] = {c, 0};
        fs_append(fs_current, redirect_file, t);
    } else {
        vga_putchar(c);
    }
}

static void prompt(void) {
    fs_pwd();
    vga_print(" # ");
}

static int parse_args(char* line, char* argv[], int max) {
    int argc = 0, in_word = 0, pos = 0;
    while (line[pos] && argc < max) {
        if (line[pos] == ' ' || line[pos] == '\t') {
            line[pos] = 0;
            in_word = 0;
        } else if (!in_word) {
            argv[argc++] = &line[pos];
            in_word = 1;
        }
        pos++;
    }
    return argc;
}

static int parse_redirect(char* cmd, char* out_file, int* mode) {
    *mode = 0;
    int len = 0;
    while (cmd[len]) len++;
    for (int i = len - 2; i >= 0; i--) {
        if (cmd[i] == '>' && cmd[i+1] == '>') {
            *mode = 2;
            cmd[i] = 0;
            int j = i + 2;
            while (cmd[j] == ' ') j++;
            int k = 0;
            while (cmd[j] && cmd[j] != ' ' && k < 63) {
                out_file[k++] = cmd[j++];
            }
            out_file[k] = 0;
            if (!fs_find_file(fs_current, out_file)) fs_mkfile(fs_current, out_file);
            return 1;
        }
    }
    for (int i = len - 1; i >= 0; i--) {
        if (cmd[i] == '>') {
            *mode = 1;
            cmd[i] = 0;
            int j = i + 1;
            while (cmd[j] == ' ') j++;
            int k = 0;
            while (cmd[j] && cmd[j] != ' ' && k < 63) {
                out_file[k++] = cmd[j++];
            }
            out_file[k] = 0;
            if (!fs_find_file(fs_current, out_file)) fs_mkfile(fs_current, out_file);
            fs_write(fs_current, out_file, "");
            return 1;
        }
    }
    return 0;
}

static void print_int(int n) {
    if (n == 0) { vga_outs("0"); return; }
    char buf[16];
    int i = 0, neg = 0;
    if (n < 0) { neg = 1; n = -n; }
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    if (neg) buf[i++] = '-';
    buf[i] = 0;
    for (int a = 0, b = i - 1; a < b; a++, b--) {
        char t = buf[a]; buf[a] = buf[b]; buf[b] = t;
    }
    vga_outs(buf);
}

static int parse_int(const char* s) {
    int n = 0;
    while (*s >= '0' && *s <= '9') {
        n = n * 10 + (*s - '0');
        s++;
    }
    return n;
}

static void cmd_help(void) {
    vga_outs("BojanOS shell - built-in commands:\n");
    vga_outs("File: cat cd cp ls ll dir mkdir rmdir rm mv touch head tail wc sort uniq diff grep find chmod chown ln stat file df du\n");
    vga_outs("Text: cut paste join split csplit tee tr sed awk bc dc expr factor seq shuf comm cmp patch sdiff basename dirname realpath readlink mktemp yes true false test [\n");
    vga_outs("Proc: ps top kill killall nice renice pgrep pkill pidof skill snice pstree pwdx watch free vmstat iostat mpstat sar pidstat htop\n");
    vga_outs("Disk: fdisk parted mkfs fsck mount umount mount.nfs mount.cifs mount.fuse losetup dmsetup cryptsetup ecryptfs tune2fs dumpe2fs debugfs e2fsck resize2fs e2label findfs blkid vol_id pvcreate lvextend lvcreate\n");
    vga_outs("Net:  ifconfig ip ping netstat ss route traceroute nslookup dig wget curl ssh ftp telnet nc nmap tcpdump openssl ssh-keygen gpg\n");
    vga_outs("User: sudo su nohup man passwd useradd userdel usermod groupadd groupdel crontab at anacron chkconfig update-rc.d rc-update\n");
    vga_outs("Sys:  systemctl service journalctl dmesg syslog logger lsmod modprobe insmod rmmod depmod modinfo sysctl procfs sysfs mknod mkfifo chgrp chattr lsattr getfacl setfacl quota quotacheck repquota sync fsync fdatasync swapon swapoff mkswap\n");
    vga_outs("Dev:  lspci lsusb lscpu lsblk lsscsi dmidecode hdparm smartctl sensors\n");
    vga_outs("Boot: reboot shutdown halt poweroff init\n");
    vga_outs("Job:  jobs fg bg\n");
    vga_outs("Edit: vi vim nano emacs sublime view pico ex sed\n");
    vga_outs("Lang: gcc make python perl ruby node go rustc\n");
    vga_outs("VCS:  git svn cvs\n");
    vga_outs("Pkg:  apt yum dnf pacman brew port rpm\n");
    vga_outs("Comp: tar gzip gunzip bzip2 bunzip2 xz zip unzip rar uuencode uudecode\n");
    vga_outs("Term: screen tmux script scriptreplay reset tput stty tty ptsname login logout exit\n");
    vga_outs("Time: date cal ncal uptime time timeout tzselect zdump zic timedatectl ntpdate ntpq chronyc\n");
    vga_outs("Locale: locale localedef iconv\n");
    vga_outs("Misc: uname hostname ver whoami who w users finger mesg wall write talk last lastb\n");
    vga_outs("Fun:  figlet toilet cowsay fortune sl cmatrix banner primes\n");
}
''')

shell.close()
print('written')
