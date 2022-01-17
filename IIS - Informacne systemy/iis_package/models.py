from iis_package import db 
from flask_login import UserMixin
import dataclasses as dc


@dc.dataclass
class Reservation(db.Model):
    id : int
    confirmed : str
    cancelled : str
    email : str
    enter_stop : str
    exit_stop : str
    user_id : int
    connection_id : int
    date_time : str
    
    id =                db.Column(db.Integer, primary_key=True)
    confirmed =         db.Column(db.Boolean)
    cancelled =         db.Column(db.Boolean)
    date_time =         db.Column(db.DateTime)
    email     =         db.Column(db.String(100))
    enter_stop =        db.Column(db.String(100), db.ForeignKey('stop.name', ondelete='CASCADE'))
    exit_stop =         db.Column(db.String(100), db.ForeignKey('stop.name', ondelete='CASCADE'))
    connection_id =     db.Column(db.Integer, db.ForeignKey('link.id'))
    user_id =           db.Column(db.Integer, db.ForeignKey('user.id'))
    seats =             db.relationship('Seat', backref='reservation', lazy=True)

@dc.dataclass
class Seat(db.Model):
    id : int
    position : int
    tarif : int
    reservation_class : int

    id =                db.Column(db.Integer, primary_key=True)
    position =          db.Column(db.Integer)
    """
    0 - kid
    1 - student
    2 - invalid
    3 - base
    """
    tarif =             db.Column(db.Integer)
    """
    0 - first class
    1 - second class
    """
    reservation_class = db.Column(db.Integer)
    reservation_id =    db.Column(db.Integer, db.ForeignKey('reservation.id'))


@dc.dataclass
class Vehicle(db.Model):
    license_plate : str
    vehicle_type : str
    first_class_capacity : int
    second_class_capacity : int
    description : str

    license_plate         = db.Column(db.String(100), primary_key=True)
    vehicle_type          = db.Column(db.String(100))
    first_class_capacity  = db.Column(db.Integer)
    second_class_capacity = db.Column(db.Integer)
    description           = db.Column(db.String(1000))
    links                 = db.relationship('Link', backref='vehicle', lazy=True)


@dc.dataclass
class Stop(db.Model):
    name : str
    address : str
    latitude : float
    longitude : float
    status : int
    description : str

    name =              db.Column(db.String(100), primary_key=True)
    address =           db.Column(db.String(100))
    latitude =          db.Column(db.Float)
    longitude =         db.Column(db.Float)
    '''
    Specifies current Stop status
    0 = proposed, waiting for approval
    1 = active
    2 = request to change
    '''
    status =            db.Column(db.Integer)
    description =       db.Column(db.String(100))
    links =             db.relationship('Link', secondary='route', back_populates='routes')


@dc.dataclass
class Route(db.Model):
    stop_id : str
    link_id : int
    arrival_time :   str
    departure_time : str

    stop_id =           db.Column(db.String(100),   db.ForeignKey('stop.name'),   primary_key=True)
    link_id =           db.Column(db.Integer,       db.ForeignKey('link.id'),   primary_key=True)
    arrival_time =      db.Column(db.Time(timezone=True))
    departure_time =    db.Column(db.Time(timezone=True))


@dc.dataclass
class Link(db.Model):
    id : int
    operator : int
    interval : int
    link_name : str
    vehicle_id : str

    id =                db.Column(db.Integer, primary_key=True)
    operator =          db.Column(db.Integer, db.ForeignKey('user.id'))
    confirmed =         db.Column(db.Boolean)
    """
    Specifies days in week when link is active
    Mo - 0b1
    Tu - 0b10
    We - 0b100
    Th - 0b1000
    Fr - 0b10000
    Sa - 0b100000
    Su - 0b1000000
    """
    link_name =         db.Column(db.String(100))
    interval =          db.Column(db.Integer)
    original_id =       db.Column(db.Integer, db.ForeignKey('link.id'))
    routes =            db.relationship('Stop', secondary='route', back_populates='links')
    vehicle_id =        db.Column(db.String(100), db.ForeignKey('vehicle.license_plate'))
    reservations =      db.relationship('Reservation', backref='connection', lazy=True)

@dc.dataclass
class Current_location(db.Model):
    id : int
    location : int
    connection_id : int

    id =                db.Column(db.Integer, primary_key=True)
    location =          db.Column(db.Integer)
    connection_id =     db.Column(db.Integer, db.ForeignKey('link.id'))

@dc.dataclass
class User(db.Model, UserMixin):
    id : int
    login_name : str
    password : str
    user_name : str
    user_surname : str
    email : str
    phone_number : str

    id =                db.Column(db.Integer, primary_key=True)
    login_name =        db.Column(db.String(100), unique=True)
    password =          db.Column(db.String(512))
    user_name =         db.Column(db.String(100))
    user_surname =      db.Column(db.String(100))
    email =             db.Column(db.String(100), unique=True)
    phone_number =      db.Column(db.String(100))
    roles =             db.relationship('Role', secondary='user_roles', back_populates='users')
    reservations =      db.relationship('Reservation', backref='user', lazy=True)


@dc.dataclass
class Role(db.Model):
    name : str

    name =              db.Column(db.String(50), primary_key=True)
    users =             db.relationship('User', secondary='user_roles', back_populates='roles')


class UserRoles(db.Model):
    user_id =           db.Column(db.Integer,       db.ForeignKey('user.id', ondelete='CASCADE'),   primary_key=True)
    role_name =         db.Column(db.String(50),    db.ForeignKey('role.name', ondelete='CASCADE'), primary_key=True)


