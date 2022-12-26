import logging

from src import settings
from src.server_frontend import app

FORMAT = "[%(funcName)s() @ %(filename)s:%(lineno)d] %(message)s"
logger = logging.getLogger(settings.logger_name)
logging.basicConfig(format=FORMAT, level=logging.DEBUG)
logging.getLogger("werkzeug").setLevel(logging.ERROR)


def main():
    logger.info("Starting up")
    logger.info(f"Address: {settings.server_addr['host']}:{settings.server_addr['port']}")

    app.run(**settings.server_addr)

    logger.info("Exiting")


if __name__ == '__main__':
    main()
