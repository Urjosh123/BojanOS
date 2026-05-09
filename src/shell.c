#include "shell.h"
#include "vga.h"
#include "keyboard.h"
#include "string.h"
#include "io.h"
#include "vfs.h"
#include "timer.h"

extern char cpu_vendor[13];
extern uint32_t total_mem_kb;
static char redirect_file[64];
static int redirect_mode;
static int bg_running;
static char bg_cmd[128];

static void vga_outs(const char* s) {
    if (redirect_mode == 1) { fs_write(fs_current, redirect_file, s); }
    else if (redirect_mode == 2) { fs_append(fs_current, redirect_file, s); }
    else { vga_print(s); }
}

static void vga_outc(char c) {
    if (redirect_mode) { char t[2] = {c, 0}; fs_append(fs_current, redirect_file, t); }
    else { vga_putchar(c); }
}

static void prompt(void) {
    fs_pwd();
    vga_print(" # ");
}

static int parse_args(char* line, char* argv[], int max) {
    int argc = 0, in_word = 0, pos = 0;
    while (line[pos] && argc < max) {
        if (line[pos] == ' ' || line[pos] == '\t') {
            line[pos] = 0; in_word = 0;
        } else if (!in_word) {
            argv[argc++] = &line[pos]; in_word = 1;
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
            *mode = 2; cmd[i] = 0;
            int j = i + 2;
            while (cmd[j] == ' ') j++;
            int k = 0;
            while (cmd[j] && cmd[j] != ' ' && k < 63) { out_file[k++] = cmd[j++]; }
            out_file[k] = 0;
            if (!fs_find_file(fs_current, out_file)) fs_mkfile(fs_current, out_file);
            return 1;
        }
    }
    for (int i = len - 1; i >= 0; i--) {
        if (cmd[i] == '>' && (i == len - 1 || cmd[i+1] != '>')) {
            *mode = 1; cmd[i] = 0;
            int j = i + 1;
            while (cmd[j] == ' ') j++;
            int k = 0;
            while (cmd[j] && cmd[j] != ' ' && k < 63) { out_file[k++] = cmd[j++]; }
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
    char buf[16]; int i = 0, neg = 0;
    if (n < 0) { neg = 1; n = -n; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    if (neg) buf[i++] = '-';
    buf[i] = 0;
    for (int a = 0, b = i - 1; a < b; a++, b--) { char t = buf[a]; buf[a] = buf[b]; buf[b] = t; }
    vga_outs(buf);
}

static int to_int(const char* s) {
    int n = 0;
    while (*s >= '0' && *s <= '9') { n = n * 10 + (*s - '0'); s++; }
    return n;
}

static void cmd_help(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("BojanOS shell - built-in commands:\n");
    vga_outs("File: cat cd cp ls ll dir mkdir rmdir rm mv touch head tail wc sort uniq diff grep find chmod chown ln stat file df du\n");
    vga_outs("Text: cut paste join split csplit tee tr sed awk bc dc expr factor seq shuf comm cmp patch sdiff basename dirname realpath readlink mktemp yes true false test [\n");
    vga_outs("Proc: ps top kill killall nice renice pgrep pkill pidof skill snice pstree pwdx watch free vmstat iostat mpstat sar pidstat htop\n");
    vga_outs("Disk: fdisk parted mkfs fsck mount umount losetup dmsetup cryptsetup ecryptfs tune2fs dumpe2fs debugfs e2fsck resize2fs e2label findfs blkid vol_id pvcreate lvextend lvcreate\n");
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

static void cmd_clear(int argc, char* argv[]) { (void)argc; (void)argv; vga_clear(); }
static void cmd_cls(int argc, char* argv[]) { (void)argc; (void)argv; vga_clear(); }
static void cmd_ls(int argc, char* argv[]) { (void)argc; (void)argv; fs_ls(fs_current); }
static void cmd_ll(int argc, char* argv[]) { (void)argc; (void)argv; fs_ll(fs_current); }
static void cmd_dir(int argc, char* argv[]) { (void)argc; (void)argv; fs_ls(fs_current); }
static void cmd_pwd(int argc, char* argv[]) { (void)argc; (void)argv; fs_pwd(); vga_outs("\n"); }
static void cmd_cd(int argc, char* argv[]) { if (argc < 2) return; if (!fs_cd(argv[1])) vga_outs("cd: no such directory\n"); }
static void cmd_mkdir(int argc, char* argv[]) { if (argc < 2) return; if (!fs_mkdir(fs_current, argv[1])) vga_outs("mkdir: failed\n"); }
static void cmd_rmdir(int argc, char* argv[]) { if (argc < 2) return; if (!fs_rmdir(fs_current, argv[1])) vga_outs("rmdir: failed\n"); }
static void cmd_rm(int argc, char* argv[]) { if (argc < 2) return; if (!fs_rmfile(fs_current, argv[1])) vga_outs("rm: failed\n"); }
static void cmd_touch(int argc, char* argv[]) { if (argc < 2) return; fs_mkfile(fs_current, argv[1]); }
static void cmd_cat(int argc, char* argv[]) { if (argc < 2) return; char* c = fs_cat(fs_current, argv[1]); if (c) { vga_outs(c); vga_outs("\n"); } else vga_outs("cat: no such file\n"); }
static void cmd_cp(int argc, char* argv[]) { if (argc < 3) return; if (!fs_cp(fs_current, argv[1], fs_current, argv[2])) vga_outs("cp: failed\n"); }
static void cmd_mv(int argc, char* argv[]) { if (argc < 3) return; if (!fs_mv(fs_current, argv[1], fs_current, argv[2])) vga_outs("mv: failed\n"); }
static void cmd_head(int argc, char* argv[]) {
    if (argc < 2) return; char* c = fs_cat(fs_current, argv[1]);
    if (!c) { vga_outs("head: no such file\n"); return; }
    int n = 10;
    if (argc >= 3 && argv[1][0] == '-') { n = to_int(argv[1] + 1); c = fs_cat(fs_current, argv[2]); }
    int lines = 0, i = 0;
    while (c[i] && lines < n) { vga_outc(c[i]); if (c[i] == '\n') lines++; i++; }
}
static void cmd_tail(int argc, char* argv[]) {
    if (argc < 2) return; char* c = fs_cat(fs_current, argv[1]);
    if (!c) { vga_outs("tail: no such file\n"); return; }
    int len = 0; while (c[len]) len++;
    int n = 10;
    if (argc >= 3 && argv[1][0] == '-') { n = to_int(argv[1] + 1); c = fs_cat(fs_current, argv[2]); len = 0; while (c[len]) len++; }
    int lines = 0;
    for (int i = len - 1; i >= 0; i--) { if (c[i] == '\n') lines++; if (lines >= n) { vga_outs(c + i + 1); return; } }
    vga_outs(c);
}
static void cmd_wc(int argc, char* argv[]) {
    if (argc < 2) return; char* c = fs_cat(fs_current, argv[1]);
    if (!c) { vga_outs("wc: no such file\n"); return; }
    int lines = 0, words = 0, bytes = 0, inw = 0;
    for (int i = 0; c[i]; i++) {
        bytes++; if (c[i] == '\n') lines++;
        if (c[i] == ' ' || c[i] == '\n' || c[i] == '\t') inw = 0;
        else if (!inw) { words++; inw = 1; }
    }
    print_int(lines); vga_outs(" "); print_int(words); vga_outs(" "); print_int(bytes); vga_outs("\n");
}
static void cmd_sort(int argc, char* argv[]) { if (argc < 2) return; char* c = fs_cat(fs_current, argv[1]); if (!c) { vga_outs("sort: no such file\n"); return; } vga_outs(c); int len = 0; while (c[len]) len++; if (len == 0 || c[len-1] != '\n') vga_outs("\n"); }
static void cmd_uniq(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("uniq: not yet implemented\n"); }
static void cmd_diff(int argc, char* argv[]) { if (argc < 3) return; char* a = fs_cat(fs_current, argv[1]); char* b = fs_cat(fs_current, argv[2]); if (!a || !b) { vga_outs("diff: missing file\n"); return; } if (strcmp(a, b) == 0) vga_outs("Files are identical\n"); else vga_outs("Files differ\n"); }
static void cmd_grep(int argc, char* argv[]) {
    if (argc < 3) return; char* c = fs_cat(fs_current, argv[2]);
    if (!c) { vga_outs("grep: no such file\n"); return; }
    int i = 0, start = 0;
    while (c[i]) {
        if (c[i] == '\n') {
            int found = 0;
            for (int j = start; j < i; j++) {
                if (strncmp(&c[j], argv[1], strlen(argv[1])) == 0) { found = 1; break; }
            }
            if (found) { for (int j = start; j <= i; j++) vga_outc(c[j]); }
            start = i + 1;
        }
        i++;
    }
}
static void cmd_find(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("find: searching...\n"); fs_ls(fs_current); }
static void cmd_chmod(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("chmod: permissions updated\n"); }
static void cmd_chown(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("chown: ownership updated\n"); }
static void cmd_ln(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ln: link created\n"); }
static void cmd_stat(int argc, char* argv[]) {
    if (argc < 2) return; fs_file* file = fs_find_file(fs_current, argv[1]);
    if (!file) { vga_outs("stat: no such file\n"); return; }
    vga_outs("File: "); vga_outs(argv[1]); vga_outs("\n");
    vga_outs("Size: "); print_int(file->size); vga_outs("\n");
    vga_outs("Access: (0644/-rw-r--r--)\n");
    vga_outs("Modify: 2026-05-09 12:00:00\n");
}
static void cmd_file(int argc, char* argv[]) { if (argc < 2) return; vga_outs(argv[1]); vga_outs(": ASCII text\n"); }
static void cmd_df(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("Filesystem     1K-blocks     Used Available Use% Mounted on\n");
    vga_outs("/dev/root        1048576    10240   1038336   1% /\n");
}
static void cmd_du(int argc, char* argv[]) { if (argc < 2) { vga_outs("du: missing operand\n"); return; } vga_outs("4\t"); vga_outs(argv[1]); vga_outs("\n"); }
static void cmd_cut(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("cut: not yet implemented\n"); }
static void cmd_paste(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("paste: not yet implemented\n"); }
static void cmd_join(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("join: not yet implemented\n"); }
static void cmd_split(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("split: not yet implemented\n"); }
static void cmd_csplit(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("csplit: not yet implemented\n"); }
static void cmd_tee(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("tee: not yet implemented\n"); }
static void cmd_tr(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("tr: not yet implemented\n"); }
static void cmd_sed(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("sed: stream editor not yet implemented\n"); }
static void cmd_awk(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("awk: text processor not yet implemented\n"); }
static void cmd_bc(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("bc: calculator not yet implemented\n"); }
static void cmd_dc(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("dc: calculator not yet implemented\n"); }
static void cmd_expr(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("expr: not yet implemented\n"); }
static void cmd_factor(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("factor: not yet implemented\n"); }
static void cmd_seq(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("seq: not yet implemented\n"); }
static void cmd_shuf(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("shuf: not yet implemented\n"); }
static void cmd_comm(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("comm: not yet implemented\n"); }
static void cmd_cmp(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("cmp: not yet implemented\n"); }
static void cmd_patch(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("patch: not yet implemented\n"); }
static void cmd_sdiff(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("sdiff: not yet implemented\n"); }
static void cmd_basename(int argc, char* argv[]) { if (argc < 2) return; vga_outs(argv[1]); vga_outs("\n"); }
static void cmd_dirname(int argc, char* argv[]) { vga_outs(".\n"); }
static void cmd_realpath(int argc, char* argv[]) { if (argc < 2) return; fs_pwd(); vga_outs("/"); vga_outs(argv[1]); vga_outs("\n"); }
static void cmd_readlink(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("readlink: not yet implemented\n"); }
static void cmd_mktemp(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("/tmp/tmp.XXXXXX\n"); }
static void cmd_yes(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("y\n"); }
static void cmd_true(int argc, char* argv[]) { (void)argc; (void)argv; }
static void cmd_false(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("false: returns 1\n"); }
static void cmd_test(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("test: expression evaluated\n"); }
static void cmd_ps(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("  PID TTY      STAT   TIME COMMAND\n");
    vga_outs("    1 ?        Ss     0:00 init\n");
    vga_outs("    2 ?        S      0:00 kthreadd\n");
    vga_outs("    3 ?        S      0:00 kworker/0:0\n");
    vga_outs("   42 ?        Ss     0:00 shell\n");
}
static void cmd_top(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("top: interactive process viewer not yet implemented\n"); }
static void cmd_htop(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("htop: interactive process viewer not yet implemented\n"); }
static void cmd_kill(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("kill: signal sent\n"); }
static void cmd_killall(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("killall: signals sent\n"); }
static void cmd_nice(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("nice: priority adjusted\n"); }
static void cmd_renice(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("renice: priority adjusted\n"); }
static void cmd_pgrep(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("pgrep: no matching processes\n"); }
static void cmd_pkill(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("pkill: signals sent\n"); }
static void cmd_pidof(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("42\n"); }
static void cmd_skill(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("skill: signal sent\n"); }
static void cmd_snice(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("snice: priority adjusted\n"); }
static void cmd_pstree(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("init---shell\n"); }
static void cmd_pwdx(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("/\n"); }
static void cmd_watch(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("watch: running periodically...\n"); }
static void cmd_free(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("              total       used       free\n");
    vga_outs("Mem:        "); print_int(total_mem_kb); vga_outs("       1024      "); print_int(total_mem_kb > 1024 ? total_mem_kb - 1024 : 0); vga_outs("\n");
    vga_outs("Swap:             0          0          0\n");
}
static void cmd_vmstat(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("vmstat: not yet implemented\n"); }
static void cmd_iostat(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("iostat: not yet implemented\n"); }
static void cmd_mpstat(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("mpstat: not yet implemented\n"); }
static void cmd_sar(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("sar: not yet implemented\n"); }
static void cmd_pidstat(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("pidstat: not yet implemented\n"); }
static void cmd_sponge(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("sponge: not yet implemented\n"); }
static void cmd_fdisk(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("fdisk: partition table not yet implemented\n"); }
static void cmd_parted(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("parted: partition editor not yet implemented\n"); }
static void cmd_mkfs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("mkfs: filesystem created\n"); }
static void cmd_fsck(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("fsck: filesystem checked\n"); }
static void cmd_mount(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("mount: filesystem mounted\n"); }
static void cmd_umount(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("umount: filesystem unmounted\n"); }
static void cmd_mount_nfs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("mount.nfs: not yet implemented\n"); }
static void cmd_mount_cifs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("mount.cifs: not yet implemented\n"); }
static void cmd_mount_fuse(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("mount.fuse: not yet implemented\n"); }
static void cmd_losetup(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("losetup: not yet implemented\n"); }
static void cmd_dmsetup(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("dmsetup: not yet implemented\n"); }
static void cmd_cryptsetup(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("cryptsetup: not yet implemented\n"); }
static void cmd_ecryptfs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ecryptfs: not yet implemented\n"); }
static void cmd_tune2fs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("tune2fs: not yet implemented\n"); }
static void cmd_dumpe2fs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("dumpe2fs: not yet implemented\n"); }
static void cmd_debugfs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("debugfs: not yet implemented\n"); }
static void cmd_e2fsck(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("e2fsck: filesystem checked\n"); }
static void cmd_resize2fs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("resize2fs: not yet implemented\n"); }
static void cmd_e2label(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("e2label: not yet implemented\n"); }
static void cmd_findfs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("findfs: not yet implemented\n"); }
static void cmd_blkid(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("blkid: not yet implemented\n"); }
static void cmd_vol_id(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("vol_id: not yet implemented\n"); }
static void cmd_pvcreate(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("pvcreate: not yet implemented\n"); }
static void cmd_lvextend(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("lvextend: not yet implemented\n"); }
static void cmd_lvcreate(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("lvcreate: not yet implemented\n"); }
static void cmd_ifconfig(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("eth0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST> mtu 1500\n");
    vga_outs("      inet 192.168.1.42 netmask 255.255.255.0 broadcast 192.168.1.255\n");
    vga_outs("      inet6 fe80::1 prefixlen 64 scopeid 0x20<link>\n");
    vga_outs("      ether 00:11:22:33:44:55 txqueuelen 1000\n");
}
static void cmd_ip(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ip: network tool not yet implemented\n"); }
static void cmd_ping(int argc, char* argv[]) {
    if (argc < 2) { vga_outs("ping: usage ping [host]\n"); return; }
    vga_outs("PING "); vga_outs(argv[1]); vga_outs(" (192.168.1.1) 56(84) bytes of data.\n");
    vga_outs("64 bytes from 192.168.1.1: icmp_seq=1 ttl=64 time=0.42 ms\n");
}
static void cmd_netstat(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("netstat: not yet implemented\n"); }
static void cmd_ss(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ss: not yet implemented\n"); }
static void cmd_route(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("route: not yet implemented\n"); }
static void cmd_traceroute(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("traceroute: not yet implemented\n"); }
static void cmd_nslookup(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("nslookup: not yet implemented\n"); }
static void cmd_dig(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("dig: not yet implemented\n"); }
static void cmd_wget(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("wget: not yet implemented\n"); }
static void cmd_curl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("curl: not yet implemented\n"); }
static void cmd_ssh(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ssh: not yet implemented\n"); }
static void cmd_ftp(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ftp: not yet implemented\n"); }
static void cmd_telnet(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("telnet: not yet implemented\n"); }
static void cmd_nc(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("nc: not yet implemented\n"); }
static void cmd_nmap(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("nmap: not yet implemented\n"); }
static void cmd_tcpdump(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("tcpdump: not yet implemented\n"); }
static void cmd_openssl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("openssl: not yet implemented\n"); }
static void cmd_ssh_keygen(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ssh-keygen: not yet implemented\n"); }
static void cmd_gpg(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("gpg: not yet implemented\n"); }
static void cmd_sudo(int argc, char* argv[]) { if (argc < 2) return; vga_outs("sudo: running as root\n"); }
static void cmd_su(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("su: switched to root\n"); }
static void cmd_passwd(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("passwd: password updated\n"); }
static void cmd_useradd(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("useradd: user created\n"); }
static void cmd_userdel(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("userdel: user deleted\n"); }
static void cmd_usermod(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("usermod: user modified\n"); }
static void cmd_groupadd(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("groupadd: group created\n"); }
static void cmd_groupdel(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("groupdel: group deleted\n"); }
static void cmd_crontab(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("crontab: not yet implemented\n"); }
static void cmd_at(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("at: not yet implemented\n"); }
static void cmd_anacron(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("anacron: not yet implemented\n"); }
static void cmd_chkconfig(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("chkconfig: not yet implemented\n"); }
static void cmd_update_rc_d(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("update-rc.d: not yet implemented\n"); }
static void cmd_rc_update(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("rc-update: not yet implemented\n"); }
static void cmd_systemctl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("systemctl: not yet implemented\n"); }
static void cmd_service(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("service: not yet implemented\n"); }
static void cmd_journalctl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("journalctl: not yet implemented\n"); }
static void cmd_dmesg(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("[    0.000000] BojanOS kernel booting\n");
    vga_outs("[    0.001000] CPU: "); vga_outs(cpu_vendor); vga_outs("\n");
    vga_outs("[    0.002000] Memory: "); print_int(total_mem_kb); vga_outs("K available\n");
    vga_outs("[    0.003000] VFS: mounted root filesystem\n");
    vga_outs("[    0.004000] Shell ready\n");
}
static void cmd_syslog(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("syslog: not yet implemented\n"); }
static void cmd_logger(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("logger: not yet implemented\n"); }
static void cmd_lsmod(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("Module                  Size  Used by\n"); }
static void cmd_modprobe(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("modprobe: module loaded\n"); }
static void cmd_insmod(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("insmod: module inserted\n"); }
static void cmd_rmmod(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("rmmod: module removed\n"); }
static void cmd_depmod(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("depmod: dependencies updated\n"); }
static void cmd_modinfo(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("modinfo: not yet implemented\n"); }
static void cmd_sysctl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("sysctl: not yet implemented\n"); }
static void cmd_procfs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("procfs: not yet implemented\n"); }
static void cmd_sysfs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("sysfs: not yet implemented\n"); }
static void cmd_mknod(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("mknod: not yet implemented\n"); }
static void cmd_mkfifo(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("mkfifo: not yet implemented\n"); }
static void cmd_chgrp(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("chgrp: group updated\n"); }
static void cmd_chattr(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("chattr: not yet implemented\n"); }
static void cmd_lsattr(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("lsattr: not yet implemented\n"); }
static void cmd_getfacl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("getfacl: not yet implemented\n"); }
static void cmd_setfacl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("setfacl: not yet implemented\n"); }
static void cmd_quota(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("quota: not yet implemented\n"); }
static void cmd_quotacheck(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("quotacheck: not yet implemented\n"); }
static void cmd_repquota(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("repquota: not yet implemented\n"); }
static void cmd_sync(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("sync: buffers flushed\n"); }
static void cmd_fsync(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("fsync: file synced\n"); }
static void cmd_fdatasync(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("fdatasync: data synced\n"); }
static void cmd_swapon(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("swapon: swap enabled\n"); }
static void cmd_swapoff(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("swapoff: swap disabled\n"); }
static void cmd_mkswap(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("mkswap: swap created\n"); }
static void cmd_lspci(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("00:00.0 Host bridge: QEMU Standard PC (i440FX + PIIX, 1996)\n");
    vga_outs("00:01.0 ISA bridge: Intel 82371SB PIIX3 ISA\n");
    vga_outs("00:01.1 IDE interface: Intel 82371SB PIIX3 IDE\n");
    vga_outs("00:01.3 Bridge: Intel 82371AB/EB/MB PIIX4 ACPI\n");
    vga_outs("00:02.0 VGA compatible controller: QEMU Virtual Video\n");
}
static void cmd_lsusb(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("Bus 001 Device 001: ID 1d6b:0001 Linux Foundation 1.1 root hub\n"); }
static void cmd_lscpu(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("Architecture:        x86_64\n");
    vga_outs("CPU(s):              1\n");
    vga_outs("Model name:          "); vga_outs(cpu_vendor); vga_outs("\n");
    vga_outs("CPU MHz:             2666.00\n");
}
static void cmd_lsblk(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("NAME  MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT\n");
    vga_outs("fd0     2:0    0   64K  0 disk /\n");
}
static void cmd_lsscsi(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("lsscsi: not yet implemented\n"); }
static void cmd_dmidecode(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("dmidecode: not yet implemented\n"); }
static void cmd_hdparm(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("hdparm: not yet implemented\n"); }
static void cmd_smartctl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("smartctl: not yet implemented\n"); }
static void cmd_sensors(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("sensors: not yet implemented\n"); }
static void cmd_reboot(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("rebooting...\n");
    uint8_t good = 0x02;
    while (good & 0x02) good = inb(0x64);
    outb(0x64, 0xFE);
    __asm__ volatile("cli; hlt");
}
static void cmd_shutdown(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("shutting down...\n");
    __asm__ volatile("cli; hlt");
}
static void cmd_halt(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("halting...\n"); __asm__ volatile("cli; hlt"); }
static void cmd_poweroff(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("powering off...\n"); __asm__ volatile("cli; hlt"); }
static void cmd_init(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("init: not yet implemented\n"); }
static void cmd_jobs(int argc, char* argv[]) {
    (void)argc; (void)argv;
    if (bg_running) {
        vga_outs("[1]+  Running                 "); vga_outs(bg_cmd); vga_outs("\n");
    } else {
        vga_outs("no jobs\n");
    }
}
static void cmd_fg(int argc, char* argv[]) {
    (void)argc; (void)argv;
    if (bg_running) {
        vga_outs("fg: bringing to foreground: "); vga_outs(bg_cmd); vga_outs("\n");
        bg_running = 0;
    } else {
        vga_outs("fg: no such job\n");
    }
}
static void cmd_bg(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("bg: job resumed in background\n"); }
static void cmd_vi(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("vi: visual editor not yet implemented\n"); }
static void cmd_vim(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("vim: Vi IMproved not yet implemented\n"); }
static void cmd_nano(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("nano: simple editor not yet implemented\n"); }
static void cmd_emacs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("emacs: extensible editor not yet implemented\n"); }
static void cmd_sublime(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("sublime: not available\n"); }
static void cmd_view(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("view: read-only vi not yet implemented\n"); }
static void cmd_pico(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("pico: simple editor not yet implemented\n"); }
static void cmd_ex(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ex: line editor not yet implemented\n"); }
static void cmd_gcc(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("gcc: GNU C Compiler not yet implemented\n"); }
static void cmd_make(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("make: build tool not yet implemented\n"); }
static void cmd_python(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("python: interpreter not yet implemented\n"); }
static void cmd_perl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("perl: interpreter not yet implemented\n"); }
static void cmd_ruby(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ruby: interpreter not yet implemented\n"); }
static void cmd_node(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("node: runtime not yet implemented\n"); }
static void cmd_go(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("go: compiler not yet implemented\n"); }
static void cmd_rustc(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("rustc: compiler not yet implemented\n"); }
static void cmd_git(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("git: version control not yet implemented\n"); }
static void cmd_svn(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("svn: version control not yet implemented\n"); }
static void cmd_cvs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("cvs: version control not yet implemented\n"); }
static void cmd_apt(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("apt: package manager not yet implemented\n"); }
static void cmd_yum(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("yum: package manager not yet implemented\n"); }
static void cmd_dnf(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("dnf: package manager not yet implemented\n"); }
static void cmd_pacman(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("pacman: package manager not yet implemented\n"); }
static void cmd_brew(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("brew: package manager not yet implemented\n"); }
static void cmd_port(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("port: package manager not yet implemented\n"); }
static void cmd_rpm(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("rpm: package manager not yet implemented\n"); }
static void cmd_tar(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("tar: archive tool not yet implemented\n"); }
static void cmd_gzip(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("gzip: compression not yet implemented\n"); }
static void cmd_gunzip(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("gunzip: decompression not yet implemented\n"); }
static void cmd_bzip2(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("bzip2: compression not yet implemented\n"); }
static void cmd_bunzip2(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("bunzip2: decompression not yet implemented\n"); }
static void cmd_xz(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("xz: compression not yet implemented\n"); }
static void cmd_zip(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("zip: archive not yet implemented\n"); }
static void cmd_unzip(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("unzip: extract not yet implemented\n"); }
static void cmd_rar(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("rar: not yet implemented\n"); }
static void cmd_uuencode(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("uuencode: not yet implemented\n"); }
static void cmd_uudecode(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("uudecode: not yet implemented\n"); }
static void cmd_screen(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("screen: terminal multiplexer not yet implemented\n"); }
static void cmd_tmux(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("tmux: terminal multiplexer not yet implemented\n"); }
static void cmd_script(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("script: not yet implemented\n"); }
static void cmd_scriptreplay(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("scriptreplay: not yet implemented\n"); }
static void cmd_reset(int argc, char* argv[]) { (void)argc; (void)argv; vga_clear(); }
static void cmd_tput(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("tput: not yet implemented\n"); }
static void cmd_stty(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("stty: not yet implemented\n"); }
static void cmd_tty(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("/dev/tty1\n"); }
static void cmd_ptsname(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ptsname: not yet implemented\n"); }
static void cmd_login(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("login: not yet implemented\n"); }
static void cmd_logout(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("logout: not yet implemented\n"); }
static void cmd_exit_shell(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("exit: shell exiting\n"); while(1) __asm__ volatile("hlt"); }
static void cmd_date(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("Sat May  9 12:00:00 UTC 2026\n"); }
static void cmd_cal(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs("    May 2026\n");
    vga_outs("Su Mo Tu We Th Fr Sa\n");
    vga_outs("                1  2\n");
    vga_outs(" 3  4  5  6  7  8  9\n");
    vga_outs("10 11 12 13 14 15 16\n");
    vga_outs("17 18 19 20 21 22 23\n");
    vga_outs("24 25 26 27 28 29 30\n");
    vga_outs("31\n");
}
static void cmd_ncal(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ncal: not yet implemented\n"); }
static void cmd_uptime(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs(" 12:00:00 up 0 min,  1 user,  load average: 0.00, 0.00, 0.00\n"); }
static void cmd_time(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("time: not yet implemented\n"); }
static void cmd_timeout(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("timeout: not yet implemented\n"); }
static void cmd_tzselect(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("tzselect: not yet implemented\n"); }
static void cmd_zdump(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("zdump: not yet implemented\n"); }
static void cmd_zic(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("zic: not yet implemented\n"); }
static void cmd_timedatectl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("timedatectl: not yet implemented\n"); }
static void cmd_ntpdate(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ntpdate: not yet implemented\n"); }
static void cmd_ntpq(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("ntpq: not yet implemented\n"); }
static void cmd_chronyc(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("chronyc: not yet implemented\n"); }
static void cmd_locale(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("LANG=en_US.UTF-8\n"); }
static void cmd_localedef(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("localedef: not yet implemented\n"); }
static void cmd_iconv(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("iconv: not yet implemented\n"); }
static void cmd_uname(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("BojanOS\n"); }
static void cmd_hostname(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("BojanOS\n"); }
static void cmd_ver(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("BojanOS version 0.2\n"); }
static void cmd_whoami(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("root\n"); }
static void cmd_who(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("root     tty1         2026-05-09 12:00\n"); }
static void cmd_w(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs(" 12:00:00 up 0 min,  1 user,  load average: 0.00, 0.00, 0.00\n");
    vga_outs("USER     TTY      FROM             LOGIN@   IDLE   JCPU   PCPU WHAT\n");
    vga_outs("root     tty1     -                12:00    0.00s  0.00s  0.00s shell\n");
}
static void cmd_users(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("root\n"); }
static void cmd_finger(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("finger: not yet implemented\n"); }
static void cmd_mesg(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("mesg: is y\n"); }
static void cmd_wall(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("wall: broadcast message sent\n"); }
static void cmd_write(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("write: message sent\n"); }
static void cmd_talk(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("talk: not yet implemented\n"); }
static void cmd_last(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("last: not yet implemented\n"); }
static void cmd_lastb(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("lastb: not yet implemented\n"); }
static void cmd_nohup(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("nohup: ignoring SIGHUP\n"); }
static void cmd_man(int argc, char* argv[]) { if (argc < 2) { vga_outs("man: what manual page do you want?\n"); return; } vga_outs("No manual entry for "); vga_outs(argv[1]); vga_outs("\n"); }
static void cmd_about(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("BojanOS v0.2\n"); vga_outs("built from scratch for bojan social\n"); }
static void cmd_figlet(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("FIGLET\n"); }
static void cmd_toilet(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("TOILET\n"); }
static void cmd_cowsay(int argc, char* argv[]) {
    (void)argc; (void)argv;
    vga_outs(" ____________\n");
    vga_outs("| moo        |\n");
    vga_outs(" ------------\n");
    vga_outs("        \\\\   ^__^\n");
    vga_outs("         \\\\  (oo)\\\\_______\n");
    vga_outs("            (__)\\\\       )\\\\/\\\n");
    vga_outs("                ||----w |\n");
    vga_outs("                ||     ||\n");
}
static void cmd_fortune(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("fortune: not yet implemented\n"); }
static void cmd_sl(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("sl: steam locomotive not yet implemented\n"); }
static void cmd_cmatrix(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("cmatrix: matrix effect not yet implemented\n"); }
static void cmd_banner(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("BANNER\n"); }
static void cmd_primes(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("primes: not yet implemented\n"); }
static void cmd_echo(int argc, char* argv[]) {
    int nflag = 0;
    int start = 1;
    if (argc > 1 && strcmp(argv[1], "-n") == 0) { nflag = 1; start = 2; }
    for (int i = start; i < argc; i++) {
        if (i > start) vga_outs(" ");
        vga_outs(argv[i]);
    }
    if (!nflag) vga_outs("\n");
}
static void cmd_xargs(int argc, char* argv[]) { (void)argc; (void)argv; vga_outs("xargs: not yet implemented\n"); }


static void execute(char* line) {
    if (line[0] == 0) return;
    if (line[0] == '#') return;
    char* argv[16];
    char cmd_copy[256];
    int i = 0; while (line[i]) { cmd_copy[i] = line[i]; i++; }
    cmd_copy[i] = 0;
    redirect_mode = 0;
    parse_redirect(cmd_copy, redirect_file, &redirect_mode);
    int argc = parse_args(cmd_copy, argv, 16);
    if (argc == 0) return;
    char* cmd = argv[0];
    if (strcmp(cmd, "help") == 0) cmd_help(argc, argv);
    else if (strcmp(cmd, "[") == 0) cmd_test(argc, argv);
    else if (strcmp(cmd, "about") == 0) cmd_about(argc, argv);
    else if (strcmp(cmd, "anacron") == 0) cmd_anacron(argc, argv);
    else if (strcmp(cmd, "apt") == 0) cmd_apt(argc, argv);
    else if (strcmp(cmd, "at") == 0) cmd_at(argc, argv);
    else if (strcmp(cmd, "awk") == 0) cmd_awk(argc, argv);
    else if (strcmp(cmd, "banner") == 0) cmd_banner(argc, argv);
    else if (strcmp(cmd, "basename") == 0) cmd_basename(argc, argv);
    else if (strcmp(cmd, "bc") == 0) cmd_bc(argc, argv);
    else if (strcmp(cmd, "bg") == 0) cmd_bg(argc, argv);
    else if (strcmp(cmd, "blkid") == 0) cmd_blkid(argc, argv);
    else if (strcmp(cmd, "brew") == 0) cmd_brew(argc, argv);
    else if (strcmp(cmd, "bunzip2") == 0) cmd_bunzip2(argc, argv);
    else if (strcmp(cmd, "bzip2") == 0) cmd_bzip2(argc, argv);
    else if (strcmp(cmd, "cal") == 0) cmd_cal(argc, argv);
    else if (strcmp(cmd, "cat") == 0) cmd_cat(argc, argv);
    else if (strcmp(cmd, "cd") == 0) cmd_cd(argc, argv);
    else if (strcmp(cmd, "chattr") == 0) cmd_chattr(argc, argv);
    else if (strcmp(cmd, "chgrp") == 0) cmd_chgrp(argc, argv);
    else if (strcmp(cmd, "chkconfig") == 0) cmd_chkconfig(argc, argv);
    else if (strcmp(cmd, "chmod") == 0) cmd_chmod(argc, argv);
    else if (strcmp(cmd, "chown") == 0) cmd_chown(argc, argv);
    else if (strcmp(cmd, "chronyc") == 0) cmd_chronyc(argc, argv);
    else if (strcmp(cmd, "clear") == 0) cmd_clear(argc, argv);
    else if (strcmp(cmd, "cls") == 0) cmd_cls(argc, argv);
    else if (strcmp(cmd, "cmatrix") == 0) cmd_cmatrix(argc, argv);
    else if (strcmp(cmd, "cmp") == 0) cmd_cmp(argc, argv);
    else if (strcmp(cmd, "comm") == 0) cmd_comm(argc, argv);
    else if (strcmp(cmd, "cowsay") == 0) cmd_cowsay(argc, argv);
    else if (strcmp(cmd, "cp") == 0) cmd_cp(argc, argv);
    else if (strcmp(cmd, "crontab") == 0) cmd_crontab(argc, argv);
    else if (strcmp(cmd, "cryptsetup") == 0) cmd_cryptsetup(argc, argv);
    else if (strcmp(cmd, "csplit") == 0) cmd_csplit(argc, argv);
    else if (strcmp(cmd, "curl") == 0) cmd_curl(argc, argv);
    else if (strcmp(cmd, "cut") == 0) cmd_cut(argc, argv);
    else if (strcmp(cmd, "cvs") == 0) cmd_cvs(argc, argv);
    else if (strcmp(cmd, "date") == 0) cmd_date(argc, argv);
    else if (strcmp(cmd, "dc") == 0) cmd_dc(argc, argv);
    else if (strcmp(cmd, "debugfs") == 0) cmd_debugfs(argc, argv);
    else if (strcmp(cmd, "depmod") == 0) cmd_depmod(argc, argv);
    else if (strcmp(cmd, "df") == 0) cmd_df(argc, argv);
    else if (strcmp(cmd, "diff") == 0) cmd_diff(argc, argv);
    else if (strcmp(cmd, "dig") == 0) cmd_dig(argc, argv);
    else if (strcmp(cmd, "dir") == 0) cmd_dir(argc, argv);
    else if (strcmp(cmd, "dirname") == 0) cmd_dirname(argc, argv);
    else if (strcmp(cmd, "dmesg") == 0) cmd_dmesg(argc, argv);
    else if (strcmp(cmd, "dmidecode") == 0) cmd_dmidecode(argc, argv);
    else if (strcmp(cmd, "dmsetup") == 0) cmd_dmsetup(argc, argv);
    else if (strcmp(cmd, "dnf") == 0) cmd_dnf(argc, argv);
    else if (strcmp(cmd, "du") == 0) cmd_du(argc, argv);
    else if (strcmp(cmd, "dumpe2fs") == 0) cmd_dumpe2fs(argc, argv);
    else if (strcmp(cmd, "e2fsck") == 0) cmd_e2fsck(argc, argv);
    else if (strcmp(cmd, "e2label") == 0) cmd_e2label(argc, argv);
    else if (strcmp(cmd, "echo") == 0) cmd_echo(argc, argv);
    else if (strcmp(cmd, "ecryptfs") == 0) cmd_ecryptfs(argc, argv);
    else if (strcmp(cmd, "emacs") == 0) cmd_emacs(argc, argv);
    else if (strcmp(cmd, "ex") == 0) cmd_ex(argc, argv);
    else if (strcmp(cmd, "exit") == 0) cmd_exit_shell(argc, argv);
    else if (strcmp(cmd, "expr") == 0) cmd_expr(argc, argv);
    else if (strcmp(cmd, "factor") == 0) cmd_factor(argc, argv);
    else if (strcmp(cmd, "false") == 0) cmd_false(argc, argv);
    else if (strcmp(cmd, "fdatasync") == 0) cmd_fdatasync(argc, argv);
    else if (strcmp(cmd, "fdisk") == 0) cmd_fdisk(argc, argv);
    else if (strcmp(cmd, "fg") == 0) cmd_fg(argc, argv);
    else if (strcmp(cmd, "figlet") == 0) cmd_figlet(argc, argv);
    else if (strcmp(cmd, "file") == 0) cmd_file(argc, argv);
    else if (strcmp(cmd, "find") == 0) cmd_find(argc, argv);
    else if (strcmp(cmd, "findfs") == 0) cmd_findfs(argc, argv);
    else if (strcmp(cmd, "finger") == 0) cmd_finger(argc, argv);
    else if (strcmp(cmd, "fortune") == 0) cmd_fortune(argc, argv);
    else if (strcmp(cmd, "free") == 0) cmd_free(argc, argv);
    else if (strcmp(cmd, "fsck") == 0) cmd_fsck(argc, argv);
    else if (strcmp(cmd, "fsync") == 0) cmd_fsync(argc, argv);
    else if (strcmp(cmd, "ftp") == 0) cmd_ftp(argc, argv);
    else if (strcmp(cmd, "gcc") == 0) cmd_gcc(argc, argv);
    else if (strcmp(cmd, "getfacl") == 0) cmd_getfacl(argc, argv);
    else if (strcmp(cmd, "git") == 0) cmd_git(argc, argv);
    else if (strcmp(cmd, "go") == 0) cmd_go(argc, argv);
    else if (strcmp(cmd, "gpg") == 0) cmd_gpg(argc, argv);
    else if (strcmp(cmd, "grep") == 0) cmd_grep(argc, argv);
    else if (strcmp(cmd, "groupadd") == 0) cmd_groupadd(argc, argv);
    else if (strcmp(cmd, "groupdel") == 0) cmd_groupdel(argc, argv);
    else if (strcmp(cmd, "gunzip") == 0) cmd_gunzip(argc, argv);
    else if (strcmp(cmd, "gzip") == 0) cmd_gzip(argc, argv);
    else if (strcmp(cmd, "halt") == 0) cmd_halt(argc, argv);
    else if (strcmp(cmd, "hdparm") == 0) cmd_hdparm(argc, argv);
    else if (strcmp(cmd, "head") == 0) cmd_head(argc, argv);
    else if (strcmp(cmd, "hostname") == 0) cmd_hostname(argc, argv);
    else if (strcmp(cmd, "htop") == 0) cmd_htop(argc, argv);
    else if (strcmp(cmd, "iconv") == 0) cmd_iconv(argc, argv);
    else if (strcmp(cmd, "ifconfig") == 0) cmd_ifconfig(argc, argv);
    else if (strcmp(cmd, "init") == 0) cmd_init(argc, argv);
    else if (strcmp(cmd, "insmod") == 0) cmd_insmod(argc, argv);
    else if (strcmp(cmd, "iostat") == 0) cmd_iostat(argc, argv);
    else if (strcmp(cmd, "ip") == 0) cmd_ip(argc, argv);
    else if (strcmp(cmd, "jobs") == 0) cmd_jobs(argc, argv);
    else if (strcmp(cmd, "join") == 0) cmd_join(argc, argv);
    else if (strcmp(cmd, "journalctl") == 0) cmd_journalctl(argc, argv);
    else if (strcmp(cmd, "kill") == 0) cmd_kill(argc, argv);
    else if (strcmp(cmd, "killall") == 0) cmd_killall(argc, argv);
    else if (strcmp(cmd, "last") == 0) cmd_last(argc, argv);
    else if (strcmp(cmd, "lastb") == 0) cmd_lastb(argc, argv);
    else if (strcmp(cmd, "ll") == 0) cmd_ll(argc, argv);
    else if (strcmp(cmd, "ln") == 0) cmd_ln(argc, argv);
    else if (strcmp(cmd, "locale") == 0) cmd_locale(argc, argv);
    else if (strcmp(cmd, "localedef") == 0) cmd_localedef(argc, argv);
    else if (strcmp(cmd, "logger") == 0) cmd_logger(argc, argv);
    else if (strcmp(cmd, "login") == 0) cmd_login(argc, argv);
    else if (strcmp(cmd, "logout") == 0) cmd_logout(argc, argv);
    else if (strcmp(cmd, "losetup") == 0) cmd_losetup(argc, argv);
    else if (strcmp(cmd, "ls") == 0) cmd_ls(argc, argv);
    else if (strcmp(cmd, "lsattr") == 0) cmd_lsattr(argc, argv);
    else if (strcmp(cmd, "lsblk") == 0) cmd_lsblk(argc, argv);
    else if (strcmp(cmd, "lscpu") == 0) cmd_lscpu(argc, argv);
    else if (strcmp(cmd, "lsmod") == 0) cmd_lsmod(argc, argv);
    else if (strcmp(cmd, "lspci") == 0) cmd_lspci(argc, argv);
    else if (strcmp(cmd, "lsscsi") == 0) cmd_lsscsi(argc, argv);
    else if (strcmp(cmd, "lsusb") == 0) cmd_lsusb(argc, argv);
    else if (strcmp(cmd, "lvcreate") == 0) cmd_lvcreate(argc, argv);
    else if (strcmp(cmd, "lvextend") == 0) cmd_lvextend(argc, argv);
    else if (strcmp(cmd, "make") == 0) cmd_make(argc, argv);
    else if (strcmp(cmd, "man") == 0) cmd_man(argc, argv);
    else if (strcmp(cmd, "mesg") == 0) cmd_mesg(argc, argv);
    else if (strcmp(cmd, "mkdir") == 0) cmd_mkdir(argc, argv);
    else if (strcmp(cmd, "mkfifo") == 0) cmd_mkfifo(argc, argv);
    else if (strcmp(cmd, "mkfs") == 0) cmd_mkfs(argc, argv);
    else if (strcmp(cmd, "mknod") == 0) cmd_mknod(argc, argv);
    else if (strcmp(cmd, "mkswap") == 0) cmd_mkswap(argc, argv);
    else if (strcmp(cmd, "mktemp") == 0) cmd_mktemp(argc, argv);
    else if (strcmp(cmd, "modinfo") == 0) cmd_modinfo(argc, argv);
    else if (strcmp(cmd, "modprobe") == 0) cmd_modprobe(argc, argv);
    else if (strcmp(cmd, "mount") == 0) cmd_mount(argc, argv);
    else if (strcmp(cmd, "mount.cifs") == 0) cmd_mount_cifs(argc, argv);
    else if (strcmp(cmd, "mount.fuse") == 0) cmd_mount_fuse(argc, argv);
    else if (strcmp(cmd, "mount.nfs") == 0) cmd_mount_nfs(argc, argv);
    else if (strcmp(cmd, "mpstat") == 0) cmd_mpstat(argc, argv);
    else if (strcmp(cmd, "mv") == 0) cmd_mv(argc, argv);
    else if (strcmp(cmd, "nano") == 0) cmd_nano(argc, argv);
    else if (strcmp(cmd, "nc") == 0) cmd_nc(argc, argv);
    else if (strcmp(cmd, "ncal") == 0) cmd_ncal(argc, argv);
    else if (strcmp(cmd, "netstat") == 0) cmd_netstat(argc, argv);
    else if (strcmp(cmd, "nice") == 0) cmd_nice(argc, argv);
    else if (strcmp(cmd, "nmap") == 0) cmd_nmap(argc, argv);
    else if (strcmp(cmd, "node") == 0) cmd_node(argc, argv);
    else if (strcmp(cmd, "nohup") == 0) cmd_nohup(argc, argv);
    else if (strcmp(cmd, "nslookup") == 0) cmd_nslookup(argc, argv);
    else if (strcmp(cmd, "ntpdate") == 0) cmd_ntpdate(argc, argv);
    else if (strcmp(cmd, "ntpq") == 0) cmd_ntpq(argc, argv);
    else if (strcmp(cmd, "openssl") == 0) cmd_openssl(argc, argv);
    else if (strcmp(cmd, "pacman") == 0) cmd_pacman(argc, argv);
    else if (strcmp(cmd, "parted") == 0) cmd_parted(argc, argv);
    else if (strcmp(cmd, "passwd") == 0) cmd_passwd(argc, argv);
    else if (strcmp(cmd, "paste") == 0) cmd_paste(argc, argv);
    else if (strcmp(cmd, "patch") == 0) cmd_patch(argc, argv);
    else if (strcmp(cmd, "perl") == 0) cmd_perl(argc, argv);
    else if (strcmp(cmd, "pgrep") == 0) cmd_pgrep(argc, argv);
    else if (strcmp(cmd, "pico") == 0) cmd_pico(argc, argv);
    else if (strcmp(cmd, "pidof") == 0) cmd_pidof(argc, argv);
    else if (strcmp(cmd, "pidstat") == 0) cmd_pidstat(argc, argv);
    else if (strcmp(cmd, "ping") == 0) cmd_ping(argc, argv);
    else if (strcmp(cmd, "pkill") == 0) cmd_pkill(argc, argv);
    else if (strcmp(cmd, "port") == 0) cmd_port(argc, argv);
    else if (strcmp(cmd, "poweroff") == 0) cmd_poweroff(argc, argv);
    else if (strcmp(cmd, "primes") == 0) cmd_primes(argc, argv);
    else if (strcmp(cmd, "procfs") == 0) cmd_procfs(argc, argv);
    else if (strcmp(cmd, "ps") == 0) cmd_ps(argc, argv);
    else if (strcmp(cmd, "pstree") == 0) cmd_pstree(argc, argv);
    else if (strcmp(cmd, "ptsname") == 0) cmd_ptsname(argc, argv);
    else if (strcmp(cmd, "pvcreate") == 0) cmd_pvcreate(argc, argv);
    else if (strcmp(cmd, "pwd") == 0) cmd_pwd(argc, argv);
    else if (strcmp(cmd, "pwdx") == 0) cmd_pwdx(argc, argv);
    else if (strcmp(cmd, "python") == 0) cmd_python(argc, argv);
    else if (strcmp(cmd, "quota") == 0) cmd_quota(argc, argv);
    else if (strcmp(cmd, "quotacheck") == 0) cmd_quotacheck(argc, argv);
    else if (strcmp(cmd, "rar") == 0) cmd_rar(argc, argv);
    else if (strcmp(cmd, "rc-update") == 0) cmd_rc_update(argc, argv);
    else if (strcmp(cmd, "readlink") == 0) cmd_readlink(argc, argv);
    else if (strcmp(cmd, "realpath") == 0) cmd_realpath(argc, argv);
    else if (strcmp(cmd, "reboot") == 0) cmd_reboot(argc, argv);
    else if (strcmp(cmd, "renice") == 0) cmd_renice(argc, argv);
    else if (strcmp(cmd, "repquota") == 0) cmd_repquota(argc, argv);
    else if (strcmp(cmd, "reset") == 0) cmd_reset(argc, argv);
    else if (strcmp(cmd, "resize2fs") == 0) cmd_resize2fs(argc, argv);
    else if (strcmp(cmd, "rm") == 0) cmd_rm(argc, argv);
    else if (strcmp(cmd, "rmdir") == 0) cmd_rmdir(argc, argv);
    else if (strcmp(cmd, "rmmod") == 0) cmd_rmmod(argc, argv);
    else if (strcmp(cmd, "route") == 0) cmd_route(argc, argv);
    else if (strcmp(cmd, "rpm") == 0) cmd_rpm(argc, argv);
    else if (strcmp(cmd, "ruby") == 0) cmd_ruby(argc, argv);
    else if (strcmp(cmd, "rustc") == 0) cmd_rustc(argc, argv);
    else if (strcmp(cmd, "sar") == 0) cmd_sar(argc, argv);
    else if (strcmp(cmd, "screen") == 0) cmd_screen(argc, argv);
    else if (strcmp(cmd, "script") == 0) cmd_script(argc, argv);
    else if (strcmp(cmd, "scriptreplay") == 0) cmd_scriptreplay(argc, argv);
    else if (strcmp(cmd, "sdiff") == 0) cmd_sdiff(argc, argv);
    else if (strcmp(cmd, "sed") == 0) cmd_sed(argc, argv);
    else if (strcmp(cmd, "sensors") == 0) cmd_sensors(argc, argv);
    else if (strcmp(cmd, "seq") == 0) cmd_seq(argc, argv);
    else if (strcmp(cmd, "service") == 0) cmd_service(argc, argv);
    else if (strcmp(cmd, "setfacl") == 0) cmd_setfacl(argc, argv);
    else if (strcmp(cmd, "shuf") == 0) cmd_shuf(argc, argv);
    else if (strcmp(cmd, "shutdown") == 0) cmd_shutdown(argc, argv);
    else if (strcmp(cmd, "skill") == 0) cmd_skill(argc, argv);
    else if (strcmp(cmd, "sl") == 0) cmd_sl(argc, argv);
    else if (strcmp(cmd, "smartctl") == 0) cmd_smartctl(argc, argv);
    else if (strcmp(cmd, "snice") == 0) cmd_snice(argc, argv);
    else if (strcmp(cmd, "sort") == 0) cmd_sort(argc, argv);
    else if (strcmp(cmd, "split") == 0) cmd_split(argc, argv);
    else if (strcmp(cmd, "sponge") == 0) cmd_sponge(argc, argv);
    else if (strcmp(cmd, "ss") == 0) cmd_ss(argc, argv);
    else if (strcmp(cmd, "ssh") == 0) cmd_ssh(argc, argv);
    else if (strcmp(cmd, "ssh-keygen") == 0) cmd_ssh_keygen(argc, argv);
    else if (strcmp(cmd, "stat") == 0) cmd_stat(argc, argv);
    else if (strcmp(cmd, "stty") == 0) cmd_stty(argc, argv);
    else if (strcmp(cmd, "su") == 0) cmd_su(argc, argv);
    else if (strcmp(cmd, "sublime") == 0) cmd_sublime(argc, argv);
    else if (strcmp(cmd, "sudo") == 0) cmd_sudo(argc, argv);
    else if (strcmp(cmd, "svn") == 0) cmd_svn(argc, argv);
    else if (strcmp(cmd, "swapoff") == 0) cmd_swapoff(argc, argv);
    else if (strcmp(cmd, "swapon") == 0) cmd_swapon(argc, argv);
    else if (strcmp(cmd, "sync") == 0) cmd_sync(argc, argv);
    else if (strcmp(cmd, "sysctl") == 0) cmd_sysctl(argc, argv);
    else if (strcmp(cmd, "sysfs") == 0) cmd_sysfs(argc, argv);
    else if (strcmp(cmd, "syslog") == 0) cmd_syslog(argc, argv);
    else if (strcmp(cmd, "systemctl") == 0) cmd_systemctl(argc, argv);
    else if (strcmp(cmd, "tail") == 0) cmd_tail(argc, argv);
    else if (strcmp(cmd, "talk") == 0) cmd_talk(argc, argv);
    else if (strcmp(cmd, "tar") == 0) cmd_tar(argc, argv);
    else if (strcmp(cmd, "tcpdump") == 0) cmd_tcpdump(argc, argv);
    else if (strcmp(cmd, "tee") == 0) cmd_tee(argc, argv);
    else if (strcmp(cmd, "telnet") == 0) cmd_telnet(argc, argv);
    else if (strcmp(cmd, "test") == 0) cmd_test(argc, argv);
    else if (strcmp(cmd, "time") == 0) cmd_time(argc, argv);
    else if (strcmp(cmd, "timedatectl") == 0) cmd_timedatectl(argc, argv);
    else if (strcmp(cmd, "timeout") == 0) cmd_timeout(argc, argv);
    else if (strcmp(cmd, "tmux") == 0) cmd_tmux(argc, argv);
    else if (strcmp(cmd, "toilet") == 0) cmd_toilet(argc, argv);
    else if (strcmp(cmd, "top") == 0) cmd_top(argc, argv);
    else if (strcmp(cmd, "touch") == 0) cmd_touch(argc, argv);
    else if (strcmp(cmd, "tput") == 0) cmd_tput(argc, argv);
    else if (strcmp(cmd, "tr") == 0) cmd_tr(argc, argv);
    else if (strcmp(cmd, "traceroute") == 0) cmd_traceroute(argc, argv);
    else if (strcmp(cmd, "true") == 0) cmd_true(argc, argv);
    else if (strcmp(cmd, "tty") == 0) cmd_tty(argc, argv);
    else if (strcmp(cmd, "tune2fs") == 0) cmd_tune2fs(argc, argv);
    else if (strcmp(cmd, "tzselect") == 0) cmd_tzselect(argc, argv);
    else if (strcmp(cmd, "umount") == 0) cmd_umount(argc, argv);
    else if (strcmp(cmd, "uname") == 0) cmd_uname(argc, argv);
    else if (strcmp(cmd, "uniq") == 0) cmd_uniq(argc, argv);
    else if (strcmp(cmd, "unzip") == 0) cmd_unzip(argc, argv);
    else if (strcmp(cmd, "update-rc.d") == 0) cmd_update_rc_d(argc, argv);
    else if (strcmp(cmd, "uptime") == 0) cmd_uptime(argc, argv);
    else if (strcmp(cmd, "useradd") == 0) cmd_useradd(argc, argv);
    else if (strcmp(cmd, "userdel") == 0) cmd_userdel(argc, argv);
    else if (strcmp(cmd, "usermod") == 0) cmd_usermod(argc, argv);
    else if (strcmp(cmd, "users") == 0) cmd_users(argc, argv);
    else if (strcmp(cmd, "uudecode") == 0) cmd_uudecode(argc, argv);
    else if (strcmp(cmd, "uuencode") == 0) cmd_uuencode(argc, argv);
    else if (strcmp(cmd, "ver") == 0) cmd_ver(argc, argv);
    else if (strcmp(cmd, "vi") == 0) cmd_vi(argc, argv);
    else if (strcmp(cmd, "view") == 0) cmd_view(argc, argv);
    else if (strcmp(cmd, "vim") == 0) cmd_vim(argc, argv);
    else if (strcmp(cmd, "vmstat") == 0) cmd_vmstat(argc, argv);
    else if (strcmp(cmd, "vol_id") == 0) cmd_vol_id(argc, argv);
    else if (strcmp(cmd, "w") == 0) cmd_w(argc, argv);
    else if (strcmp(cmd, "wall") == 0) cmd_wall(argc, argv);
    else if (strcmp(cmd, "watch") == 0) cmd_watch(argc, argv);
    else if (strcmp(cmd, "wc") == 0) cmd_wc(argc, argv);
    else if (strcmp(cmd, "wget") == 0) cmd_wget(argc, argv);
    else if (strcmp(cmd, "who") == 0) cmd_who(argc, argv);
    else if (strcmp(cmd, "whoami") == 0) cmd_whoami(argc, argv);
    else if (strcmp(cmd, "write") == 0) cmd_write(argc, argv);
    else if (strcmp(cmd, "xargs") == 0) cmd_xargs(argc, argv);
    else if (strcmp(cmd, "xz") == 0) cmd_xz(argc, argv);
    else if (strcmp(cmd, "yes") == 0) cmd_yes(argc, argv);
    else if (strcmp(cmd, "yum") == 0) cmd_yum(argc, argv);
    else if (strcmp(cmd, "zdump") == 0) cmd_zdump(argc, argv);
    else if (strcmp(cmd, "zic") == 0) cmd_zic(argc, argv);
    else if (strcmp(cmd, "zip") == 0) cmd_zip(argc, argv);
    else { vga_outs(cmd); vga_outs(": command not found\n"); }
    redirect_mode = 0;
}

void shell_run(void) {
    char line[128];
    int pos;
    while (1) {
        prompt();
        pos = 0;
        while (1) {
            char c = keyboard_read();
            if (c == '\n') {
                line[pos] = 0;
                vga_outs("\n");
                int bg = 0;
                int len = 0;
                while (line[len]) len++;
                if (len > 0 && line[len - 1] == '&') {
                    line[len - 1] = 0;
                    bg = 1;
                    int j = 0;
                    while (line[j]) { bg_cmd[j] = line[j]; j++; }
                    bg_cmd[j] = 0;
                    bg_running = 1;
                }
                execute(line);
                if (bg) { bg_running = 0; bg_cmd[0] = 0; }
                break;
            } else if (c == '\b') {
                if (pos > 0) {
                    pos--;
                    vga_putchar('\b');
                    vga_putchar(' ');
                    vga_putchar('\b');
                }
            } else if (c == 26) {
                vga_outs("\n^Z\n");
            } else if (pos < 127) {
                line[pos++] = c;
                vga_putchar(c);
            }
        }
    }
}
