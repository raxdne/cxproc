#
#
#

for D in calendar dir each plain system xhtml xml xsl
do
    find . -iname "test-$D*.cxp" -exec xmllint --noout --relaxng ../xml/schema/cxp.rng {} \; 2> xmllint-$D.log
done

find . -iname "config.cxp" -exec xmllint --noout --relaxng ../xml/schema/cxp.rng {} \; 2> xmllint-config.log
