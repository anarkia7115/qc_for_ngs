aws s3 cp ../bin/qc s3://gcbibucket/copy/qc
ssh caws aws s3 cp s3://gcbibucket/copy/qc /gcbi/product/sequencing-analyze/QualityCheck/bin/

scp ./bin/qc caws:/gcbi/product/sequencing-analyze/QualityCheck/bin/
ssh caws chmod u+x /gcbi/product/sequencing-analyze/QualityCheck/bin/*

