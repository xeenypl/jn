# What is jn?
jn is a simple cli json parser for using in scripts.

# Why using jn over jq
jq is a lot bigger and more blout but also more powerful,
if you need advance json processing definitely use jq,
but if you need just json parse jn may be better option.

# how to use
You just giw as command line arguments key and index of array like this:
```sh
jn example.json list 1
```
for 
```json
{
  "list" : [
    "one",
    "two",
    "three"
  ]
}
```
and well display `tow`

# how to install
just type:
```sh
git clone http://github.com/xeenypl/jn
cd ./jn
# and as root
./install.sh
```
