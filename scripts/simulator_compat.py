# Host-only workaround for an upstream simulator static-initialization crash.
Import('env')

from pathlib import Path
import subprocess

project = Path(env.subst("$PROJECT_DIR"))
dep = project / '.pio' / 'libdeps' / env['PIOENV'] / 'simulator'
patch = project / 'scripts' / 'simulator-simlog-init.patch'
expected = '6730fe5fe576c9d924e44096a9e267d8538d184c'


def git(*args, stdin=None):
    return subprocess.run(['git', '-C', str(dep), *args], input=stdin,
                          capture_output=True, text=True)


def git_apply(*flags, stdin):
    # Zero-context hunks need --unidiff-zero. --ignore-whitespace tolerates
    # CRLF in the WSL-mounted worktree; the 100755 mode only makes git warn.
    return git('apply', *flags, '--unidiff-zero', '--ignore-whitespace', '-',
               stdin=stdin)


head = git('rev-parse', 'HEAD')
if head.returncode or head.stdout.strip() != expected:
    raise RuntimeError(f'Unexpected simulator revision at {dep}: {head.stdout.strip()} {head.stderr.strip()}')

# A Windows checkout stores the patch with CRLF, which git apply does not strip.
text = patch.read_text()
if git_apply('--reverse', '--check', stdin=text).returncode == 0:
    print('Simulator compatibility patch already applied')
else:
    check = git_apply('--check', stdin=text)
    if check.returncode:
        raise RuntimeError(f'Simulator compatibility patch cannot apply: {check.stderr.strip()}')
    apply = git_apply(stdin=text)
    if apply.returncode:
        raise RuntimeError(f'Simulator compatibility patch failed: {apply.stderr.strip()}')
    print('Applied simulator static-initialization compatibility patch')
