from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_login import LoginManager
from datetime import timedelta
from os import path

db = SQLAlchemy()

class db_info:
    USERNAME = 'YT9oxq5r8R'
    PASSWD = 'Ff70bWw7Zf'
    SERVER = 'remotemysql.com'
    NAME = 'YT9oxq5r8R'
    STRING = f'{USERNAME}:{PASSWD}@{SERVER}/{NAME}'


def create_app():
    app = Flask(__name__)
    app.config['SECRET_KEY'] = '5C988F29AE236A15797DF0C6C5B6399313FC11D871A902E53A4B0B730B86823F494BEBD14A594B0FF3C1A12AA0C92250C6DE0D39F8572D049A21D362617247B7'
    app.config['SQLALCHEMY_DATABASE_URI'] = f'mysql://{db_info.USERNAME}:{db_info.PASSWD}@{db_info.SERVER}/{db_info.NAME}'
    app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
    app.config['PERMANENT_SESSION_LIFETIME'] =  timedelta(minutes=30)
    app.config['SQLALCHEMY_ENGINE_OPTIONS'] = {'pool_size' : 100, 'pool_recycle' : 280, 'pool_pre_ping' : True}
    db.init_app(app)
    
    from .views import views
    from .auth import auth
    from .models import User
    from .passenger import passenger
    from .administrator import administrator
    from .personnel import personnel
    from .operator import operator
    from .generator import generator
    
    # blueprint view registration
    app.register_blueprint(views, url_prefix='/')
    app.register_blueprint(auth, url_prefix='/')
    app.register_blueprint(generator, url_prefix='/')
    
    # person view blueprint registration
    app.register_blueprint(passenger, url_prefix='/passenger/')
    app.register_blueprint(administrator, url_prefix='/administrator/')
    app.register_blueprint(personnel, url_prefix='/personnel/')
    app.register_blueprint(operator, url_prefix='/operator/')
    
    login_manager = LoginManager()
    login_manager.login_view = 'auth.login'
    login_manager.init_app(app)

    @login_manager.user_loader
    def load_user(user_id):
        return User.query.filter_by(id=user_id).first()

    create_database(app)
    return app

def create_database(app):
    if not path.exists('iis_package/' + db_info.STRING):
        db.create_all(app=app)


