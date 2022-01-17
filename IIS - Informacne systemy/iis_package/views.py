from flask import Blueprint, render_template, request, flash, redirect, url_for, session, jsonify, Response
from .auth import user_privileges
from flask_login import current_user
import dataclasses as dc
import datetime
views = Blueprint('views', __name__)


@dc.dataclass
class UserReservation:
    id : int
    datetime : str
    enter_stop : str
    exit_stop : str
    seats : int
    seat_positions : str
    link_name : str
    location : str

@views.route('/')
def base():
    return render_template('doc.html')

@views.route('/search')
def search():
    return render_template('passenger_views/search_connection.html')

@views.route('/search_reservations')
def search_reservations():

    if current_user.is_authenticated:
        return render_template('my_reservations.html', email=current_user.email)
    else:
        email = request.args.get('email')
        return render_template('search_reservations.html', email=email)
        

def get_positions(seats):
    tarifs = ["K", "S", "I", "B"]
    vehicle_classes = ["1st", "2nd"]
    positions = [f"{vehicle_classes[seat.reservation_class]}-P{seat.position}-{tarifs[seat.tarif]}" for seat in seats]
    return ", ".join(positions)

@views.route('/load_unconfirmed')
def load_unconfirmed_reservations():
    from .models import Reservation, Link, Seat, Current_location
    if current_user.is_authenticated:
        unconfirmed = Reservation.query.filter_by(confirmed=False, cancelled=False, user_id=current_user.id).all()
    else:
        email = request.args.get('email')
        unconfirmed = Reservation.query.filter_by(confirmed=False, cancelled=False, email=email).all()
    data = list()
    for line in unconfirmed:
        location = "Unknown"
        curr_location = Current_location.query.filter_by(connection_id=line.connection_id).first()
        if curr_location:
            link = Link.query.filter_by(id=line.connection_id).first()
            if link:
                location = str(link.routes[curr_location.location].name)
        datetime = line.date_time.strftime("%d/%m/%Y")
        seat = Seat.query.filter_by(reservation_id=line.id).all()
        positions = get_positions(seat)
        connection = Link.query.filter_by(id=line.connection_id).first()
        if seat and connection:
            data.append(UserReservation(int(line.id),datetime, line.enter_stop, line.exit_stop, len(seat), positions, str(connection.link_name), location))       
    if data:
        return jsonify({"data": data})
    else:
        return jsonify({"data": []})

@views.route('/load_denied')
def load_denied_reservations():
    from .models import Reservation, Link, Seat, Current_location
    if current_user.is_authenticated:
        denied = Reservation.query.filter_by(confirmed=False, cancelled=True, user_id=current_user.id).all()
    else:
        email = request.args.get('email')
        denied = Reservation.query.filter_by(confirmed=False, cancelled=True, email=email).all()
    data = list()
    for line in denied:
        location = "Unknown"
        curr_location = Current_location.query.filter_by(connection_id=line.connection_id).first()
        if curr_location:
            link = Link.query.filter_by(id=line.connection_id).first()
            if link:
                location = str(link.routes[curr_location.location].name)
        datetime = line.date_time.strftime("%d/%m/%Y")
        seat = Seat.query.filter_by(reservation_id=line.id).all()
        positions = get_positions(seat)
        connection = Link.query.filter_by(id=line.connection_id).first()
        if seat and connection:
            data.append(UserReservation(int(line.id), datetime, line.enter_stop, line.exit_stop, len(seat), positions, str(connection.link_name), location))    
            
    if denied:
        return jsonify({"data": data})
    else:
        return jsonify({"data": []})


@views.route('/load_allowed')
def load_allowed_reservations():
    from .models import Reservation, Link, Seat, Current_location
    if current_user.is_authenticated:
        allowed = Reservation.query.filter_by(confirmed=True, cancelled=False, user_id=current_user.id).all()
    else:
        email = request.args.get('email')
        allowed = Reservation.query.filter_by(confirmed=True, cancelled=False, email=email).all()
    data = list()
    for line in allowed:
        location = "Unknown"
        curr_location = Current_location.query.filter_by(connection_id=line.connection_id).first()
        if curr_location:
            link = Link.query.filter_by(id=line.connection_id).first()
            if link:
                location = str(link.routes[curr_location.location].name)
    
        datetime = line.date_time.strftime("%d/%m/%Y")
        seat = Seat.query.filter_by(reservation_id=line.id).all()
        positions = get_positions(seat)
        connection = Link.query.filter_by(id=line.connection_id).first()
        if seat and connection:   
            data.append(UserReservation(int(line.id),datetime, line.enter_stop, line.exit_stop, len(seat), positions, str(connection.link_name), location))    
    if allowed:
        return jsonify({"data": data})
    else:
        return jsonify({"data": []})


@views.route('/display_reservations', methods=["GET"])
def display_reservations():
    from .models import Reservation
    email = request.args.get('email')
    return render_template('my_reservations.html', email=request.args.get('email'))


@views.route('/my_reservations', methods=["POST"])
def my_reservations():
    from .models import Reservation
    if request.method == "POST":
        email = request.form.get('insertedemail')
        user_reservations = Reservation.query.filter_by(user_id=None, email=email).all()
        if not user_reservations:
            return jsonify({'error': "Invalid e-mail, reservation with this e-mail does not exist."}), 210
        else:
            return jsonify({"email": email}), 200
        

@views.route('/cancel_reservation', methods=['GET'])
def cancel_reservation():
    from .models import Reservation
    from iis_package import db
    if not request.args['id']:
        return Response(render_template('my_reservations.html'), status=210)
    else:
        reservation = Reservation.query.filter_by(id=int(request.args['id'])).first()
        reservation.cancelled = True
        db.session.commit()
        return Response(render_template('my_reservations.html'), status=200)


@views.route('/transfer_user_reservations', methods=["POST"])
def transfer_user_reservations():
    email = request.form.get("login_email")
    from .models import Reservation, User
    from iis_package import db
    user = User.query.filter_by(email=email).first()
    user_reservations = Reservation.query.filter_by(email=email).all()
    for reservation in user_reservations:
        reservation.email = None
        reservation.user_id = user.id
    db.session.commit()
    return jsonify({'email': email}), 200

@views.route('/validate_request', methods=["POST"])
def validate_request():
    from .models import Reservation
    email = request.form.get('insertedemail')
    user_reservations = Reservation.query.filter_by(user_id=None, email=email).all()
    if not user_reservations:
        return jsonify({'error': "Invalid e-mail, reservation with this e-mail does not exist."}), 210
    else:
        return jsonify({"email": email}), 200