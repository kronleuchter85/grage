


install:
	make install -C grage-linux-testing-unit
	make install -C grage-linux-testing-integration
	make install -C grage-commons
	make install -C grage-nipc
#	make build   -C grage-pfs
#	make build   -C grage-praid
#	make build   -C grage-ppd
	make install -C grage-nipc

build:
	make install -C grage-linux-testing-unit
	make install -C grage-linux-testing-integration
	make install -C grage-commons
	make build-lib -C grage-ppd
	make build-lib -C grage-praid
	make build-lib -C grage-pfs

clean:
	make clean -C grage-commons
	make clean -C grage-pfs
	make clean -C grage-praid
	make clean -C grage-ppd
	make clean -C grage-nipc
	make clean -C grage-ppd-console

seba:
	docs/install_repo.sh seba docs/template1.xsl
	make clean install -C grage-linux-testing-unit	
	make clean install -C grage-linux-testing-integration
	make clean install -C grage-commons
	make clean install -C grage-nipc	
	make clean install -C grage-ppd-console	
	make clean build -C grage-pfs
	make clean build -C grage-ppd
	make clean build -C grage-praid

joaco:
	grage-docs/install_repo.sh joaquin template1.xsl
	make clean install -C grage-linux-testing-unit	
	make clean install -C grage-linux-testing-integration
	make clean install -C grage-commons
	make clean install -C grage-nipc	
	make clean install -C grage-ppd-console	
	make clean build -C grage-pfs
	make clean build -C grage-ppd
	make clean build -C grage-praid




