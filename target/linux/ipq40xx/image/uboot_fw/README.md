## Customizing partitions
To customize partition table, `nor-plus-nand-partition.xml` needs to be edited. 

- In the partition XML the size can be given as `size_kb` or `size_block`
    options. If `size_kb` is given then last attribute should be `0xFF`,
    `size_block` should have `0xFE` as the last attribute.

- The partition `0:SBL1` cannot be reordered and it always should be the first entry.

## Creating the system partition binary

After editing the XML, create system partition binary (you can back up the existing `norplusnand-system-partition-ipq40xx.bin` file):

1. Run `python nand_mbn_generator.py nor-plus-nand-partition.xml usrtbl.bin`. First argument for `nand_mbn_generator.py` is the partition description XML file, second is the output user partition table.

2. Run `./partition_tool -u usrtbl.bin -s 256 -p 256 -b 512 -c 1 -o norplusnand-system-partition-ipq40xx.bin` to create the system partition table binary. `-u` option takes the user partition table created previously, `-s` is page size in bytes, `-p` is pages per block, `-b` - is block count, `-c` is MBIB copies, `-o` is the output binary.

You can remove `usrtbl.bin` after. Build new firmware image as usual, flash it using the bootloader (firmware recovery through httpd or other).

