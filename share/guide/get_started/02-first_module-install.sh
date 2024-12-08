#!/bin/bash
set -v

mkdir -p /apps/bar-{1.0,2.1}/bin
mkdir -p /apps/foo-{0.9,1.6}/bin

for i in 1.0 2.1; do
cat <<EOF >/apps/bar-$i/bin/bar
#!/bin/bash
echo 'bar v$i'
EOF
done

for i in 0.9 1.6; do
cat <<EOF >/apps/foo-$i/bin/foo
#!/bin/bash
echo 'foo v$i'
EOF
done

chmod +x /apps/{foo,bar}-*/bin/*
